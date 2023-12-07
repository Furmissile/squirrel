/*
  This file handles the retrieval of guild info if the user is in a guild

  Components added:
    [0] -> Join War button
    [1] -> Members (default when scurry is not at war)
    [2] -> War Participation (default when scurry is at war)
*/

struct sd_scurry_data
{
  // set data in an array to prevent disorganization
  // must be calloc'd because size CANNOT be predicted (can be any value <= 10)
  struct sd_db_info *row_data;
  int db_rows;

  // how many requests have been completed
  int response_counter;
  int is_war_rankings;
};

struct sd_scurry_info 
{
  // structs must be calloc'd anyway because they are still needed in other functions
  struct sd_player *player;
  struct sd_scurry *scurry;

  struct sd_scurry_data *scurry_data;
  PGresult* scurry_members;

  struct discord_component buttons[4];
  char custom_ids[4][64];
  int button_size; // how many buttons are displayed

  struct discord_embed_field fields[2];
  char field_names[2][64];
  char field_values[2][1024];

  char footer_text[64];
  char footer_url[128];

  int is_participation_list; // used to check if participation list should be generated
};

void build_info_buttons(const struct discord_interaction *event, struct sd_scurry_info *params, struct sd_player *event_player)
{
  struct sd_scurry *scurry = params->scurry;

  params->scurry_members = SQL_query(params->scurry_members, 
      "select * from public.player where scurry_id = %ld", scurry->scurry_owner_id);

  if (event->member->user->id == scurry->scurry_owner_id)
  {
    params->buttons[0] = (scurry->war_flag == 0) ?
      (struct discord_component)
      {
        .type = DISCORD_COMPONENT_BUTTON,
        .style = DISCORD_BUTTON_SUCCESS,
        .label = "Join War",
        .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]),
            "%c3*%ld*.%ld", TYPE_SCURRY_INFO, scurry->scurry_owner_id, event->member->user->id),
        // if war acorns isnt full or the button was pressed disable button
        .disabled = (scurry->war_acorns < scurry->war_acorn_cap
          || PQntuples(params->scurry_members) < SCURRY_MEMBER_REQ
          )
          ? true : false
      } :
      (struct discord_component)
      {
        .type = DISCORD_COMPONENT_BUTTON,
        .style = DISCORD_BUTTON_DANGER,
        .label = "Retreat",
        .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]),
            "%c3*%ld*.%ld", TYPE_SCURRY_INFO, scurry->scurry_owner_id, event->member->user->id),
      };

    params->button_size++;
  }

  // button_idx defaults to member list if scurry is not at war and participation list when at war
  int button_idx = (params->is_participation_list) ? params->button_size +1 : params->button_size;

  params->buttons[params->button_size] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "Members",
    .custom_id = u_snprintf(params->custom_ids[params->button_size], sizeof(params->custom_ids[params->button_size]),
        "%c1*%ld*.%ld", TYPE_SCURRY_INFO, scurry->scurry_owner_id, event->member->user->id),
  };

  if (button_idx == params->button_size)
  {
    params->buttons[params->button_size].style = DISCORD_BUTTON_SUCCESS;
    params->buttons[params->button_size].disabled = true;
  }
  else {
    params->buttons[params->button_size].style = DISCORD_BUTTON_PRIMARY;
  }

  params->button_size++;

  params->buttons[params->button_size] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "War Ranks",
    .custom_id = u_snprintf(params->custom_ids[params->button_size], sizeof(params->custom_ids[params->button_size]),
        "%c2*%ld*.%ld", TYPE_SCURRY_INFO, scurry->scurry_owner_id, event->member->user->id),
  };

  if (button_idx == params->button_size)
  {
    params->buttons[params->button_size].style = DISCORD_BUTTON_SUCCESS;
    params->buttons[params->button_size].disabled = true;
  }
  else {
    params->buttons[params->button_size].style = DISCORD_BUTTON_PRIMARY;
  }

  params->button_size++;

  // player must be part of scurry but NOT be the scurry owner
  if (event_player->scurry_id == scurry->scurry_owner_id
      && event_player->user_id != scurry->scurry_owner_id)
  {
    params->buttons[params->button_size] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_DANGER,
      .label = "LEAVE",
      .custom_id = u_snprintf(params->custom_ids[params->button_size], sizeof(params->custom_ids[params->button_size]),
          "%c4*%ld*.%ld", TYPE_SCURRY_LEAVE, scurry->scurry_owner_id, event->member->user->id),
    };

    params->button_size++;
  }
  // suggest invite!
  else if (event_player->scurry_id == 0)
  {
    params->buttons[params->button_size] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_SUCCESS,
      .label = "Request Invite",
      .custom_id = u_snprintf(params->custom_ids[params->button_size], sizeof(params->custom_ids[params->button_size]),
          "%c5*%ld*.%ld", TYPE_INIT_INVITE, scurry->scurry_owner_id, event->member->user->id),
    };

    if (scurry->war_flag == 1)
    {
      params->buttons[params->button_size].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[params->button_size].disabled = true;
    }
    else {
      params->buttons[params->button_size].style = DISCORD_BUTTON_PRIMARY;
    }

    params->button_size++;
  }

  PQclear(params->scurry_members);
}

void complete_scurry_interaction(
  const struct discord_interaction *event,
  struct sd_scurry_info *params)
{
  struct sd_scurry *scurry = params->scurry;
  struct sd_player *player = params->player;

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player->color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), scurry->scurry_name),
    .fields = &(struct discord_embed_fields) {
      .array = params->fields,
      .size = 2
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to %s!", scurry->scurry_name),
      .icon_url = u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path)
    }
  };

  struct sd_player event_player = { 0 };
  load_player_struct(&event_player, event->member->user->id, event->data->custom_id);
  energy_regen(&event_player);

  build_info_buttons(event, params, &event_player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params->buttons,
        .size = params->button_size
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
      }
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components)
      {
        .array = action_rows,
        // add utils if requested player is part of scurry
        .size = (event_player.scurry_id == scurry->scurry_owner_id) ? 2 : 1
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}

void list_scurry_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  struct sd_scurry_info *params = resp->data;
  struct sd_scurry *scurry = params->scurry;
  struct sd_player *player = params->player;
  struct sd_scurry_data *scurry_data = params->scurry_data;
  const struct discord_interaction *event = resp->keep;

  // if owner: UNDERLINE
  if (user->id == scurry->scurry_owner_id)
    u_snprintf(scurry_data->row_data[scurry_data->response_counter].username, 
        sizeof(scurry_data->row_data[scurry_data->response_counter].username), 
        " "INDENT" "LIST_LEADER" __%s__", 
        user->username);
  // if player: highlight
  else if (user->id == player->user_id)
    u_snprintf(scurry_data->row_data[scurry_data->response_counter].username, 
        sizeof(scurry_data->row_data[scurry_data->response_counter].username), 
        " "INDENT" "LIST_YOU" <@%ld>", 
        user->id);
  // use codeblock
  else
    u_snprintf(scurry_data->row_data[scurry_data->response_counter].username, 
        sizeof(scurry_data->row_data[scurry_data->response_counter].username), 
        " "INDENT" "LIST_MEMBER" `%s`", 
        user->username);
  
  scurry_data->response_counter++;

  if (scurry_data->response_counter == scurry_data->db_rows)
  {
    // the query is done being used at this point
    PQclear(params->scurry_members);

    // load scurry_data into scurry_info field
    if (params->is_participation_list) // this is generating war rankings
    {
      for (int idx = 0; idx < scurry_data->db_rows; idx++)
      {
        APPLY_NUM_STR(row_value, scurry_data->row_data[idx].value);
        u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
            "%s **%s** \n",
            scurry_data->row_data[idx].username, row_value);
      }
    }
    else { // this is the members embed
      // find owner to list first
      for (int idx = 0; idx < scurry_data->db_rows; idx++)
        if (scurry->scurry_owner_id == scurry_data->row_data[idx].user_id)
          u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
              "%s \n",
              scurry_data->row_data[idx].username);

      // scan the rest (make sure the owner isnt added twice!)
      for (int idx = 0; idx < scurry_data->db_rows; idx++)
        if (scurry->scurry_owner_id != scurry_data->row_data[idx].user_id)
          u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
              "%s \n",
              scurry_data->row_data[idx].username);
    }
    params->fields[1].value = params->field_values[1];

    complete_scurry_interaction(event, params);

    free(params->player);
    free(params->scurry);
    free(params->scurry_data);
    free(resp->data);
  }
}

void create_member_listing( 
  const struct discord_interaction *event, 
  struct sd_scurry_info *params)
{
  params->scurry_data = calloc(1, sizeof(struct sd_scurry_data));
  struct sd_scurry_data *scurry_data = params->scurry_data;

  scurry_data->db_rows = PQntuples(params->scurry_members);
  scurry_data->row_data = calloc(scurry_data->db_rows, sizeof(struct sd_db_info));

  for (int idx = 0; idx < scurry_data->db_rows; idx++)
  {
    scurry_data->row_data[idx].user_id = strtobigint(PQgetvalue(params->scurry_members, idx, 0));

    if (params->is_participation_list)
      scurry_data->row_data[idx].value = strtoint(PQgetvalue(params->scurry_members, idx, 1));
  
    struct discord_ret_user ret_user = {
      .done = &list_scurry_user,
      .data = params,
      .keep = event    
    };

    // queue up the users
    discord_get_user(client, scurry_data->row_data[idx].user_id, &ret_user);
  }
}

/*
To generate the member list, one of the following conditions must be true:
  > initializing the embed and scurry war IS NOT active
  > button pressed and idx == 1

To generate the participation list:
  > initializing the embed and scurry was IS active
  > button pressed == 2
*/

void fill_general_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;
  struct sd_scurry_info *params = resp->data;
  struct sd_scurry *scurry = params->scurry;

  params->fields[0].name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Scurry Details");

  APPLY_NUM_STR(total_stolen_acorns, scurry->total_stolen_acorns);
  APPLY_NUM_STR(war_acorns, scurry->war_acorns);
  APPLY_NUM_STR(war_acorn_cap, scurry->war_acorn_cap);

  params->fields[0].value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]),
      " "INDENT" "LEADER" Guild Owner: **%s** \n"
      " "INDENT" "GUILD_ICON" Scurry War: %s \n"
      " "INDENT" "WAR_ACORNS" Stolen Acorns: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Current Rank: **%s** (x**%0.1f** "ACORNS" Acorns) \n"
      " "INDENT" "LOST_STASH" War Stash: **%s**/%s \n",
      user->username, 
      (scurry->war_flag == 1) ? " "QUEST_MARKER" *Active*" : " "HELP_MARKER" *Inactive*",
      total_stolen_acorns,
      (scurry->rank == SEED_NOT) ? "Seed Nots"
      : (scurry->rank == ACORN_SNATCHER) ? "Acorn Snatchers"
      : (scurry->rank == SEED_SNIFFER) ? "Seed Sniffers"
      : (scurry->rank == OAKFFICIAL) ? "Oakfficials" : "Royal Nuts",
      BASE_ACORN_MULT * (scurry->rank +1) +1,
      war_acorns, war_acorn_cap );
  
  // members button varies on whether the join war/retreat button exists!

  int button_idx = event->data->custom_id[1] -48;

  if ((button_idx == ERROR_STATUS && scurry->war_flag == 1) // this is from Scurry info user app
    || button_idx == 2 // this is scurry related button
    || (button_idx == 5 && scurry->war_flag == 1)  // coming from info!
    || button_idx == 3)
    // this condition is taken by the form of params->is_participation_list
  {
    params->fields[1].name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Participation");
    params->is_participation_list = 1;

    // fetch all scurry members who have participated
    params->scurry_members = SQL_query(params->scurry_members, 
        "select user_id, stolen_acorns from public.player where scurry_id = %ld and stolen_acorns > 0 order by stolen_acorns desc", 
        scurry->scurry_owner_id);
    
    // if none, state such
    if (PQntuples(params->scurry_members) == 0)
    {
      params->fields[1].value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), " "OFF_ARROW" No members have participated yet!");

      complete_scurry_interaction(event, params);
    }
    // otherwise, list them
    else
      create_member_listing(event, params);
  }
  else {
    params->fields[1].name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Members");

    // fetch all the scurry members
    params->scurry_members = SQL_query(params->scurry_members, "select user_id from public.player where scurry_id = %ld", 
        scurry->scurry_owner_id);

    // list them
    create_member_listing(event, params);
  }
}

void scurry_info(const struct discord_interaction *event, struct sd_scurry_info *params)
{
  params->scurry_members = (PGresult*) { 0 };
  struct sd_scurry *scurry = params->scurry;

  // if war button was pressed
  if (event->data->custom_id[1] -48 == 3)
  {
    if (scurry->war_flag)
      scurry->war_flag = 0;
    else {
      scurry->war_flag = 1;

      PGresult* war_participants = (PGresult*) { 0 };
      war_participants = SQL_query(war_participants, "update public.player set stolen_acorns = 0 where scurry_id = %ld", 
          scurry->scurry_owner_id);
      PQclear(war_participants);

      scurry->prev_stolen_acorns = scurry->total_stolen_acorns;
      scurry->total_stolen_acorns = 0;
    }

    update_scurry_row(scurry);
  }

  struct discord_ret_user ret_user = {
    .done = &fill_general_info,
    .data = params,
    .keep = event
  };

  discord_get_user(client, params->scurry->scurry_owner_id, &ret_user);
}

int s_info_interaction(const struct discord_interaction *event)
{
  struct sd_player *player = calloc(1, sizeof(struct sd_player));
  struct sd_scurry *scurry = calloc(1, sizeof(struct sd_scurry));

  char scurry_id_buffer[64] = { };
  trim_buffer(scurry_id_buffer, sizeof(scurry_id_buffer), event->data->custom_id, '*');
  unsigned long scurry_id = strtobigint(scurry_id_buffer);

  PGresult* search_scurry = (PGresult*) { 0 };
  search_scurry = SQL_query(search_scurry, "select * from public.scurry where owner_id = %ld", scurry_id);

  if (PQntuples(search_scurry) == 0)
  {
    error_message(event, "This scurry no longer exists!");
    free(player);
    free(scurry);
    return ERROR_STATUS;
  }
  PQclear(search_scurry);

  load_scurry_struct(scurry, scurry_id);
  load_player_struct(player, event->member->user->id, event->data->custom_id);

  struct sd_scurry_info *params = calloc(1, sizeof(struct sd_scurry_info));
  params->player = player;
  params->scurry = scurry;

  scurry_info(event, params);

  return 0;
}