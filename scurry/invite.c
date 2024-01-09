struct sd_invite_info
{
  struct sd_scurry *scurry;

  char invite_request[256];
  char invite_status[256];

  unsigned long owner_id;
  unsigned long invite_user_id;
  unsigned long channel_id;
  unsigned long message_id;
};

void invite_info_cleanup(struct sd_invite_info *params)
{
  free(params->scurry);
  free(params);
}

void invite_expired(const struct discord_interaction *event, struct sd_invite_info *params)
{
  char* invite_user = event->message->embeds->array[0].author->name;

  struct sd_header_params header = { 0 };
  header.embed = (struct discord_embed)
  {
    .color = (int)ACTION_UNDEFINED,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->user->id, event->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Invite Expired!"),
    .description = u_snprintf(params->invite_request, sizeof(params->invite_request), 
        "**%s**'s invite has expired!",
        invite_user)
  };

  char msg_content[256] = { };
  //edit message
  discord_edit_message(client, event->channel_id, event->message->id, 
    &(struct discord_edit_message)
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), "**Invite Read**"),
      .embeds = &(struct discord_embeds)
      {
        .array = &header.embed,
        .size = 1
      },

      .components = &(struct discord_components) { 0 }
    },
    NULL);
  
  invite_info_cleanup(params);
}

void invite_response(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{  
  const struct discord_interaction *event = resp->keep;
  struct sd_invite_info *params = resp->data;

  // the owner's response
  struct sd_header_params invite_request = { 0 };
  // the update to the user
  struct sd_header_params invite_status = { 0 };

  if (event->data->custom_id[1] -48 == 0)
  {
    PGresult* update_player = (PGresult*) { 0 };
    update_player = SQL_query(update_player, "update public.player set scurry_id = %ld where user_id = %ld", 
        event->user->id, params->invite_user_id);
    PQclear(update_player);

    invite_request.embed = (struct discord_embed)
    {
      .color = (int)ACTION_SUCCESS,
      .author = &(struct discord_embed_author) {
        .name = u_snprintf(invite_request.username, sizeof(invite_request.username), event->user->username),
        .url = u_snprintf(invite_request.avatar_url, sizeof(invite_request.avatar_url), 
            "https://cdn.discordapp.com/avatars/%lu/%s.png",
            event->user->id, event->user->avatar)
      },
      .title = "User Accepted!",
      .description = u_snprintf(params->invite_request, sizeof(params->invite_request),
          "You have accepted **%s**.", user->username)
    };

    invite_status.embed = (struct discord_embed)
    {
      .color = (int)ACTION_SUCCESS,
      .author = &(struct discord_embed_author) {
        .name = u_snprintf(invite_status.username, sizeof(invite_status.username), event->user->username),
        .url = u_snprintf(invite_status.avatar_url, sizeof(invite_status.avatar_url), 
            "https://cdn.discordapp.com/avatars/%lu/%s.png",
            user->id, user->avatar)
      },
      .title = "Request Accepted!",
      .description = u_snprintf(params->invite_status, sizeof(params->invite_status),
          "**%s** has accepted your invite request to **%s**.",
          event->user->username, params->scurry->scurry_name)
    };
  }
  else {
    invite_request.embed = (struct discord_embed)
    {
      .color = (int)ACTION_FAILED,
      .author = &(struct discord_embed_author) {
        .name = u_snprintf(invite_request.username, sizeof(invite_request.username), event->user->username),
        .url = u_snprintf(invite_request.avatar_url, sizeof(invite_request.avatar_url), 
            "https://cdn.discordapp.com/avatars/%lu/%s.png",
            event->user->id, event->user->avatar)
      },
      .title = "User Denied!",
      .description = u_snprintf(params->invite_request, sizeof(params->invite_request),
          "You have denied **%s**.", user->username)
    };

    invite_status.embed = (struct discord_embed)
    {
      .color = (int)ACTION_FAILED,
      .author = &(struct discord_embed_author) {
        .name = u_snprintf(invite_status.username, sizeof(invite_status.username), event->user->username),
        .url = u_snprintf(invite_status.avatar_url, sizeof(invite_status.avatar_url), 
            "https://cdn.discordapp.com/avatars/%lu/%s.png",
            user->id, user->avatar)
      },
      .title = "Request Denied!",
      .description = u_snprintf(params->invite_status, sizeof(params->invite_status),
          "**%s** has denied your invite request to **%s**.",
          event->user->username, params->scurry->scurry_name)
    };
  }

  // respond to the owner here
  discord_edit_message(client, event->channel_id, params->message_id,
    &(struct discord_edit_message)
    {
      .content = "**Invite Read**",
      .embeds = &(struct discord_embeds)
      {
        .array = &invite_request.embed,
        .size = 1
      },
      .components = &(struct discord_components) { 0 }
    },
    NULL);

  char status_content[256] = { };
  discord_create_message(client, params->channel_id,
    &(struct discord_create_message)
    {
      .content = u_snprintf(status_content, sizeof(status_content),
          "<@%ld> **Invite Returned!**", user->id),
      .embeds = &(struct discord_embeds)
      {
        .array = &invite_status.embed,
        .size = 1
      },
      .components = &(struct discord_components) { 0 }
    },
    NULL);

  PGresult* delete_invite = (PGresult*) { 0 };
  delete_invite = SQL_query(delete_invite, "delete from public.invites where request_id = %ld", 
      params->message_id);
  PQclear(delete_invite);

  invite_info_cleanup(params);
}

// the owner will trigger this interaction and takes place entirely in DM
int invite_interaction(const struct discord_interaction *event)
{
  unsigned long user_id = (event->member) ? event->member->user->id : event->user->id;
  struct sd_player player = { 0 };
  load_player_struct(&player, user_id, event->data->custom_id);

  // delete EXPIRED invites
  PGresult* clean_invites = (PGresult*) { 0 };
  clean_invites = SQL_query(clean_invites, "delete from public.invites where t_stamp < %ld", time(NULL) - INVITE_CD);
  PQclear(clean_invites);

  PGresult* pending_invite = (PGresult*) { 0 };
  pending_invite = SQL_query(pending_invite, 
      "select * from public.invites where request_id = %ld", event->message->id);

  struct sd_invite_info *params = calloc(1, sizeof(struct sd_invite_info));
  params->scurry = calloc(1, sizeof(struct sd_scurry));

  if (PQntuples(pending_invite) == 0)
  {
    invite_expired(event, params);
    PQclear(pending_invite);
    return ERROR_STATUS;
  }

  load_scurry_struct(params->scurry, player.scurry_id);
  if (params->scurry->war_flag == 1)
  {
    error_message(event, "You cannot accept invites while at war! Please retreat or finish the war first.");
    invite_info_cleanup(params);
    return ERROR_STATUS;
  }

  // used for sending status back to user through the channel they sent the request
  params->channel_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_CHANNEL));
  params->owner_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_USER));
  params->message_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_MSG));
  params->invite_user_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_USER));
  PQclear(pending_invite);

  struct discord_ret_user ret_user = {
    .done = &invite_response,
    .data = params,
    .keep = event
  };

  discord_get_user(client, params->owner_id, &ret_user);

  return 0;
}