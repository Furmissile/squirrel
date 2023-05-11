/*
  This file handles the retrieval of guild info if the user is in a guild

  Components added:
    [0] -> Join War button
    [1] -> Members (default)
    [2] -> War Participation
    [3] -> Super Encounter progress (BETA)
*/

PGresult* scurry_members;

enum SCURRY_FORMAT 
{
  SCURRY_GENERAL,
  SCURRY_RANKINGS, // always where members are listed
  SCURRY_SIZE
};

struct discord_components* build_info_buttons(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = 3;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  int button_idx = (event->data->custom_id) ? event->data->custom_id[1] - 48 : 1;

  scurry_members = SQL_query(DB_ACTION_SEARCH, "select * from public.player where scurry_id = %ld", scurry.scurry_owner_id);

  buttons->array[0] = (scurry.war_flag == 0) ?
  (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Join War",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_SCURRY_INFO, scurry.scurry_owner_id),
    // if war acorns isnt full or the button was pressed disable button
    .disabled = (event->data->custom_id
      || scurry.war_acorns < scurry.war_acorn_cap
      // || PQntuples(scurry_members) < SCURRY_MEMBER_REQ)
    )
      ? true : false
  } :
  (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_DANGER,
    .label = "Retreat",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_SCURRY_INFO, scurry.scurry_owner_id),
    // if the button was pressed, disable button
    .disabled = (event->data->custom_id) ? true : false
  };

  PQclear(scurry_members);

  buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 1) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "Members",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c1_%ld", TYPE_SCURRY_INFO, event->member->user->id),
    .disabled = (button_idx == 1) ? true : false
  };

  buttons->array[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx != 1) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "War Ranks",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c2_%ld", TYPE_SCURRY_INFO, event->member->user->id),
    .disabled = (button_idx != 1) ? true : false
  };

  return buttons;
}

void complete_interaction(
  struct discord *client,
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = discord_msg->buttons
  };

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, 
    &(struct discord_edit_original_interaction_response)
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }, 
    NULL);

  update_scurry_row(scurry);
}


void fill_members_field(
  struct discord *client,
  const struct discord_interaction *event,
  struct sd_user_data *user_data)
{
  struct sd_message *discord_msg = user_data->discord_msg;
  struct discord_embed *embed = discord_msg->embed;

  char scurry_member_list[SIZEOF_FIELD_VALUE] = {};

  if (event->data->custom_id && event->data->custom_id[1] - 48 != 1) // this is generating war rankings
  {
    for (int idx = 0; idx < user_data->db_rows; idx++)
        ADD_TO_BUFFER(scurry_member_list, SIZEOF_FIELD_VALUE, "%s **%s** \n", 
            user_data->row_data[idx].username, num_str(user_data->row_data[idx].value));
  }
  else { // this is the members embed
    // find owner to list first
    for (int idx = 0; idx < user_data->db_rows; idx++)
      if (scurry.scurry_owner_id == user_data->row_data[idx].user_id)
        ADD_TO_BUFFER(scurry_member_list, SIZEOF_FIELD_VALUE, "%s \n", user_data->row_data[idx].username);

    // scan the rest (make sure the owner isnt added twice!)
    for (int idx = 0; idx < user_data->db_rows; idx++)
      if (scurry.scurry_owner_id != user_data->row_data[idx].user_id)
        ADD_TO_BUFFER(scurry_member_list, SIZEOF_FIELD_VALUE, "%s \n", user_data->row_data[idx].username);

  }

  embed->fields->array[SCURRY_RANKINGS].value = format_str(SIZEOF_FIELD_VALUE, scurry_member_list);
  complete_interaction(client, event, discord_msg);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg->buttons);
  free(discord_msg);

  PQclear(scurry_members);
}

void list_scurry_user(
  struct discord *client, 
  struct discord_response *resp, 
  const struct discord_user *user)
{
  struct sd_user_data *user_data = resp->data;
  const struct discord_interaction *event = resp->keep;

  if (user->id == scurry.scurry_owner_id)
    user_data->row_data[user_data->response_counter].username = format_str(SIZEOF_TITLE, " "INDENT" "LIST_LEADER" __%s__", user->username);
  else if (user->id == player.user_id)
    user_data->row_data[user_data->response_counter].username = format_str(SIZEOF_TITLE, " "INDENT" "LIST_YOU" <@%ld>", user->id);
  else
    user_data->row_data[user_data->response_counter].username = format_str(SIZEOF_TITLE, " "INDENT" "LIST_MEMBER" `%s`", user->username);
  
  printf("%s \n\n", user_data->row_data[user_data->response_counter].username);
  user_data->response_counter++;

  if (user_data->response_counter == user_data->db_rows)
  {
    fill_members_field(client, event, user_data);
    free(resp->data);
  }
}

void create_member_listing(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  struct sd_user_data *user_data = calloc(1, sizeof(struct sd_user_data));
  user_data->discord_msg = calloc(1, sizeof(struct sd_message));

  user_data->db_rows = PQntuples(scurry_members);
  user_data->row_data = calloc(user_data->db_rows, sizeof(struct DB_Info));

  user_data->discord_msg = discord_msg;

  for (int idx = 0; idx < user_data->db_rows; idx++)
  {
    user_data->row_data[idx].user_id = strtobigint(PQgetvalue(scurry_members, idx, 0));

    if (event->data->custom_id && event->data->custom_id[1] - 48 != 1)
      user_data->row_data[idx].value = strtoint(PQgetvalue(scurry_members, idx, 1));
  
    struct discord_ret_user ret_user = {
      .done = &list_scurry_user,
      .data = user_data,
      .keep = event
    };

    discord_get_user(client, user_data->row_data[idx].user_id, &ret_user);
  }
}

void scurry_info(
  struct discord *client, 
  struct discord_response *resp, 
  const struct discord_interaction_response *ret)
{
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct sd_message *discord_msg = resp->data;
  struct discord_embed *embed = discord_msg->embed;

  if (event->data->custom_id && event->data->custom_id[1] -48 == 0 && scurry.war_flag == 0)
  {
    scurry.war_flag = 1;
    SQL_query(DB_ACTION_UPDATE, "update public.player set war_acorns = 0 where scurry_id = %ld", scurry.scurry_owner_id);
    scurry.courage = 0;
  }
  else if (event->data->custom_id && event->data->custom_id[1] -48 == 0 && scurry.war_flag == 1)
  {
    printf("TRUEE \n");
    scurry_war_update(event);
  }

  embed->color = player.color;

  embed->title = format_str(SIZEOF_TITLE, scurry.scurry_name);

  discord_msg->buttons = build_info_buttons(event);

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = SCURRY_SIZE;
  embed->fields->array = calloc(SCURRY_SIZE, sizeof(struct discord_embed_field));

  /* Fill general scurry stats field */
  embed->fields->array[SCURRY_GENERAL].name = format_str(SIZEOF_TITLE, "Scurry Details");
  embed->fields->array[SCURRY_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "INDENT" "LEADER" Guild Owner: <@%ld> \n"
      " "INDENT" "COURAGE" Courage: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Current Rank: **%s** (x**%0.1f** "ACORNS" Acorns) \n"
      " "INDENT" "LOST_STASH" War Stash: **%s**/%s \n",
      scurry.scurry_owner_id, num_str(scurry.courage),
      (scurry.rank == SEED_NOT) ? "Seed Nots"
      : (scurry.rank == ACORN_SNATCHER) ? "Acorn Snatchers"
      : (scurry.rank == SEED_SNIFFER) ? "Seed Sniffers"
      : (scurry.rank == OAKFFICIAL) ? "Oakfficials" : "Royal Nuts",
      (BASE_COURAGE_MULT * (scurry.rank +1)) +1,
      num_str(scurry.war_acorns), num_str(scurry.war_acorn_cap) );

  // if initializing the embed OR the member listing button was pressed: generate the member list
  if (!event->data->custom_id
    || (event->data->custom_id && event->data->custom_id[1] - 48 == 1))
  {
    embed->fields->array[SCURRY_RANKINGS].name = format_str(SIZEOF_TITLE, "Members");

    scurry_members = SQL_query(DB_ACTION_SEARCH, "select user_id from public.player where scurry_id = %ld", 
        scurry.scurry_owner_id);

    create_member_listing(client, event, discord_msg);
  }
  // if the "Join War" button is pressed, default to the rankings
  else {
    embed->fields->array[SCURRY_RANKINGS].name = format_str(SIZEOF_TITLE, "Participation");

    scurry_members = SQL_query(DB_ACTION_SEARCH, "select user_id, war_acorns from public.player where scurry_id = %ld and war_acorns > 0 order by war_acorns desc", 
        scurry.scurry_owner_id);
    
    if (PQntuples(scurry_members) == 0)
    {
      embed->fields->array[SCURRY_RANKINGS].value = format_str(SIZEOF_FIELD_VALUE, " "OFF_ARROW" No members have participated yet!");
      complete_interaction(client, event, discord_msg);
      PQclear(scurry_members);
    }
    else
      create_member_listing(client, event, discord_msg);
  }

  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "/scurry_help | For Scurry details");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
}

/* Called on scurry_invite command */
int s_info_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  // if the first button is being pressed and this user isnt the owner...
  ERROR_INTERACTION((event->data->custom_id 
    && event->data->custom_id[1] == 0
    && player.user_id != scurry.scurry_owner_id), 
      "You do not have the permission to press this button!");

  player = load_player_struct(event->member->user->id);

  if (event->data->options) // load a different scurry by name if it exists
  {
    PGresult* get_scurry = SQL_query(DB_ACTION_SEARCH, "select * from public.scurry where s_name like '%s'", event->data->options->array[0].value);
    ERROR_DATABASE_RET((PQntuples(get_scurry) == 0), "This scurry doesn't exist!", get_scurry);

    scurry = load_scurry_struct(strtobigint(PQgetvalue(get_scurry, 0, DB_SCURRY_OWNER_ID)));
  }
  else { // load player scurry if they're in one
    ERROR_INTERACTION((player.scurry_id == 0), "You are not in a scurry yet!");
    scurry = load_scurry_struct(player.scurry_id);
  }

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  struct discord_ret_interaction_response ret_response = {
    .done = &scurry_info,
    .data = discord_msg,
    .keep = event
  };

  discord_create_interaction_response(client, event->id, event->token, 
    &(struct discord_interaction_response)
    {
      .type = (event->data->custom_id) ? DISCORD_INTERACTION_DEFERRED_UPDATE_MESSAGE : DISCORD_INTERACTION_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .embeds = &(struct discord_embeds) 
        {
          .array = discord_msg->embed,
          .size = 1
        }
      }
    },
    &ret_response);

  return 0;
}