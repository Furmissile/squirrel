/*

  This file handles verifying a player to join a scurry
  1. User creates slash command w. mentioned owner
    * Create interaction response to user w. success status
    * Create message in owner DM w. buttons attached
    * Obtain message context to store into db

  2. Owner respond w. button press
    * Edit DM message with update status
    * Create message in channel of slash command w. reply results

*/

struct sd_init_invite_info
{
  struct discord_component buttons[2];
  char custom_ids[2][64];
  char labels[2][64];

  char invite_request[256];
  char invite_status[256];

  unsigned long owner_id;
};

void update_invite_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct sd_init_invite_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = (int)ACTION_SUCCESS,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },

    .title = u_snprintf(header.title, sizeof(header.title), "Successful Request"),
    .description = u_snprintf(params->invite_status, sizeof(params->invite_status), 
        "Your request was successfully sent to **%s**!", user->username)
  };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  free(resp->data);
}

// handles interaction sent back to user
void invite_success(struct discord *client, struct discord_response *resp, const struct discord_message *message)
{
  struct sd_init_invite_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  PGresult* create_request = (PGresult*) { 0 };
  create_request = SQL_query(create_request, "INSERT INTO public.invites VALUES(%ld, %ld, %ld, %ld, %ld)",
      params->owner_id, event->member->user->id, message->id, event->channel_id, time(NULL));
  PQclear(create_request);

  struct discord_ret_user ret_user = {
    .done = &update_invite_user,
    .data = params,
    .keep = event
  };

  discord_get_user(client, params->owner_id, &ret_user);
}

void send_invite_dm(struct discord *client, struct discord_response *resp, const struct discord_channel *channel)
{
  struct sd_init_invite_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;
  
  struct discord_ret_message ret_msg = {
    .done = &invite_success,
    .data = params,
    .keep = event
  };

  struct sd_header_params header = { 0 };

  // handles message sent to owner at dm channel
  header.embed = (struct discord_embed) 
  {
    .color = (int)ACTION_SUCCESS,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },

    .title = u_snprintf(header.title, sizeof(header.title), "User Invite"),
    .description = u_snprintf(params->invite_request, sizeof(params->invite_request), 
        "**%s** has requested to join your scurry!", event->member->user->username),
  };

  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Accept",
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]),
        "%c0.%ld", TYPE_INVITE, params->owner_id)
  };
  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_DANGER,
    .label = "Decline",
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]),
        "%c1.%ld", TYPE_INVITE, params->owner_id)
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params->buttons,
      .size = 2
    }
  };

  char msg_content[256] = { };

  // creates message at dm channel
  discord_create_message(client, channel->id,
    &(struct discord_create_message)
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), "<@%ld> **Pending Request!**", 
          params->owner_id),

      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) 
      {
        .array = &action_rows,
        .size = 1
      }
    }, 
    &ret_msg);
}

// the user will trigger this interaction and take place entirely in the server channel
int init_invite_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  // delete EXPIRED invites
  PGresult* clean_invites = (PGresult*) { 0 };
  clean_invites = SQL_query(clean_invites, "delete from public.invites where t_stamp < %ld", time(NULL) - INVITE_CD);
  PQclear(clean_invites);

  // check scurry status and existence
  char scurry_id_buffer[64] = { };
  trim_buffer(scurry_id_buffer, sizeof(scurry_id_buffer), event->data->custom_id, '*');
  unsigned long scurry_id = strtobigint(scurry_id_buffer);

  PGresult* scurry_info = (PGresult*) { 0 };
  scurry_info = SQL_query(scurry_info, "select * from public.scurry where owner_id = %ld", scurry_id);
  
  DATABASE_ERROR((PQntuples(scurry_info) == 0), "Sorry, this scurry doesn't exist!", scurry_info);
  
  // check if scurry is at war
  DATABASE_ERROR((strtoint(PQgetvalue(scurry_info, 0, DB_WAR_FLAG)) == 1), 
      "This scurry is currently at war! Please wait until they are ready.", scurry_info);
  
  unsigned long owner_id = strtobigint(PQgetvalue(scurry_info, 0, DB_SCURRY_OWNER_ID));
  PQclear(scurry_info);

  ERROR_INTERACTION((player.scurry_id == owner_id), "You own this scurry, silly!");
  ERROR_INTERACTION((player.scurry_id > 0), "You are already in a scurry! Try leaving your current scurry.");

  // check scurry capacity
  scurry_info = SQL_query(scurry_info, "select * from public.player where scurry_id = %ld", owner_id);
  DATABASE_ERROR((PQntuples(scurry_info) == SCURRY_MEMBER_MAX), "This scurry is full!", scurry_info);
  PQclear(scurry_info);

  // check if user has a pending invite
  scurry_info = SQL_query(scurry_info, "select * from public.invites where player_id = %ld", event->member->user->id);
  DATABASE_ERROR((PQntuples(scurry_info) > 0), "You already have a pending request!", scurry_info);
  PQclear(scurry_info);

  update_player_row(&player, BASE_CD);

  struct sd_init_invite_info *params = calloc(1, sizeof(struct sd_init_invite_info));
  params->owner_id = owner_id;
  
  // get owner id for DM channel
  struct discord_create_dm dm_params = { .recipient_id = owner_id };
  struct discord_ret_channel dm_ret = { 
    .done = &send_invite_dm,
    .data = params,
    .keep = event
  };

  // Return channel context for DM channel id
  discord_create_dm(client, &dm_params, &dm_ret);

  return 0;
}