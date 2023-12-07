int disband_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  char* input = event->data->options->array[0].value;

  // check if the name is unique
  PGresult* check_scurry = (PGresult*) { 0 };

  check_scurry = SQL_query(check_scurry, "select * from public.scurry where s_name like '%s'", input);

  char error_msg[64] = { };
  u_snprintf(error_msg, sizeof(error_msg), "Sorry, the name \"%s\" doesn't exist!", input);
  ERROR_DATABASE_RET((PQntuples(check_scurry) == 0), error_msg, check_scurry);

  unsigned long scurry_id = strtobigint(PQgetvalue(check_scurry, 0, DB_SCURRY_OWNER_ID));
  PQclear(check_scurry);
  
  // covers both 0 and different id
  ERROR_INTERACTION((player.scurry_id != scurry_id), "You do not own this scurry!");

  PGresult* rename_scurry = (PGresult*) { 0 };

  rename_scurry = SQL_query(rename_scurry, 
      "delete from public.scurry where owner_id = %ld; "
      "update public.player set scurry_id = 0 where scurry_id = %ld;",
       player.scurry_id, player.scurry_id);
  PQclear(rename_scurry);

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
        "Your scurry **%s** has been permanently disbanded! \n", input)
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

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}