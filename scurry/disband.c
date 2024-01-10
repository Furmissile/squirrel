int disband_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  char* input = event->data->options->array[0].value;

  // check if the name is unique
  PGresult* check_scurry = (PGresult*) { 0 };

  check_scurry = SQL_query(check_scurry, "select * from public.scurry where name like '%s'", input);

  char error_msg[64] = { };
  u_snprintf(error_msg, sizeof(error_msg), "Sorry, the name \"%s\" doesn't exist!", input);

  DATABASE_ERROR((PQntuples(check_scurry) == 0), error_msg, check_scurry);

  unsigned long scurry_id = strtobigint(PQgetvalue(check_scurry, 0, DB_SCURRY_OWNER_ID));
  PQclear(check_scurry);
  
  // covers both 0 and different id
  ERROR_INTERACTION((player.user_id != scurry_id), "You do not own this scurry!");

  PGresult* rename_scurry = (PGresult*) { 0 };

  rename_scurry = SQL_query(rename_scurry, 
      "delete from public.scurry where owner_id = %ld; "
      "update public.player set scurry_id = 0, stolen_acorns = 0 where scurry_id = %ld;",
       scurry_id, scurry_id);
  PQclear(rename_scurry);

  // Check if the scurry still exists
  check_scurry = SQL_query(check_scurry, "select * from public.scurry where owner_id = %ld", scurry_id);

  DATABASE_ERROR((PQntuples(check_scurry) > 0),
      "An error has occurred while disbanding this scurry. \n"
      "Please report this error to the support server! (link on last page of player help)", check_scurry);
  PQclear(check_scurry);

  struct sd_header_params header = { 0 };

  char description[256] = { };

  header.embed = (struct discord_embed) 
  {
    .color = ACTION_SUCCESS,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Scurry successfully disbanded!"),
    .description = u_snprintf(description, sizeof(description),
        "Your scurry **%s** has been disbanded! \n", input)
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

  update_player_row(&player, NO_CD);

  return 0;
}