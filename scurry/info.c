/*

  This file handles the retrieval of guild info if the user is in a guild

*/

enum SCURRY_FORMAT 
{
  SCURRY_GENERAL,
  SCURRY_RANKINGS,
  SCURRY_SIZE
};

struct discord_component* build_war_button(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));
  buttons->array = calloc(1, sizeof(struct discord_component));
  buttons->size = 1;

  PGresult* scurry_members = SQL_query(conn, "select * from public.player where scurry_id = %ld", scurry.scurry_owner_id);

  buttons->array[0] = (scurry.war_flag == 0) ?
  (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Join War",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_SCURRY_WAR, scurry.scurry_owner_id),
    // if war acorns isnt full or the button was pressed disable button
    .disabled = (event->data->custom_id
      || scurry.war_acorns < scurry.war_acorn_cap
      || PQntuples(scurry_members) < SCURRY_MEMBER_REQ)
      ? true : false
  } :
  (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_DANGER,
    .label = "Retreat",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_SCURRY_WAR, scurry.scurry_owner_id),
    // if the button was pressed, disable button
    .disabled = (event->data->custom_id) ? true : false
  };

  struct discord_component *action_rows = calloc(1, sizeof(struct discord_component));

  action_rows->type = DISCORD_COMPONENT_ACTION_ROW;
  action_rows->components = buttons;

  PQclear(scurry_members);

  return action_rows;
}


int scurry_info(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  ERROR_INTERACTION((!event->data->custom_id && player.scurry_id == 0), "You are not in a scurry yet!");

  ERROR_INTERACTION((event->data->custom_id && player.user_id != scurry.scurry_owner_id), 
      "You do not have the permission to press this button!");

  if (event->data->custom_id && scurry.war_flag == 0)
  {
    scurry.war_flag = 1;
    SQL_query(conn, "update public.player set stolen_acorns = 0 where scurry_id = %ld", scurry.scurry_owner_id);
    scurry.courage = 0;
  }
  else if (event->data->custom_id && scurry.war_flag == 1)
    scurry_war_update(event);

  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  embed->title = format_str(SIZEOF_TITLE, scurry.scurry_name);

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = SCURRY_SIZE;
  embed->fields->array = calloc(SCURRY_SIZE, sizeof(struct discord_embed_field));

  /* Fill general scurry stats field */
  embed->fields->array[SCURRY_GENERAL].name = format_str(SIZEOF_TITLE, "Scurry Details");
  embed->fields->array[SCURRY_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "INDENT" "LEADER" Guild Owner: <@!%ld> \n"
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

  /* Fill the rankings field */
  char ranking_pos[SIZEOF_FIELD_VALUE] = {};

  embed->fields->array[SCURRY_RANKINGS].name = format_str(SIZEOF_TITLE, "Participation");

  PGresult* rankings = SQL_query(conn, "select * from public.player where scurry_id = %ld order by stolen_acorns desc", scurry.scurry_owner_id);

  int total_score = 0;
  for (int i = 0; i < PQntuples(rankings); i++) 
  {
    int stolen_acorns = strtoint(PQgetvalue(rankings, i, DB_STOLEN_ACORNS));
    ADD_TO_BUFFER(ranking_pos, SIZEOF_FIELD_VALUE,
        " "INDENT" %s <@%ld> **%s** \n",
        (i < 3) ? STAHR : (i < 8) ? SILVER_STAHR : BRONZE_STAHR, 
        strtobigint(PQgetvalue(rankings, i, DB_USER_ID)),
        num_str(stolen_acorns) );
    
    total_score += stolen_acorns;
  }

  ADD_TO_BUFFER(ranking_pos, SIZEOF_FIELD_VALUE, "\nTotal Score: **%s**", num_str(total_score) );

  PQclear(rankings);

  embed->fields->array[SCURRY_RANKINGS].value = format_str(SIZEOF_FIELD_VALUE, ranking_pos);

  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "/scurry_help | For Scurry details");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);

  return 0;
}

/* Called on scurry_invite command */
int s_info_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{

  player = load_player_struct(event->member->user->id);
  if (event->data->options)
  {
    PGresult* get_scurry = SQL_query(conn, "select * from public.scurry where s_name like '%s'", event->data->options->array[0].value);
    ERROR_DATABASE_RET((PQntuples(get_scurry) == 0), "This scurry doesn't exist!", get_scurry);

    scurry = load_scurry_struct(strtobigint(PQgetvalue(get_scurry, 0, DB_SCURRY_OWNER_ID)));
  }
  else
    scurry = load_scurry_struct(player.scurry_id);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  //if an error is returned, an interaction was already generated
  if ( scurry_info(event, discord_msg) == ERROR_STATUS ) 
    return 0;

  struct discord_interaction_response interaction = 
  {
    // function will always generate a new message
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = discord_msg->content,
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      },

      .components = &(struct discord_components) {
        .array = build_war_button(event),
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg->buttons);
  free(discord_msg);

  update_scurry_row(scurry);

  return 0;
}