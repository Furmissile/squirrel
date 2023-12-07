struct sd_rename_scurry
{
  char description[256];
  char footer_text[64];
  char footer_url[128];
};

int rename_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  char* input = event->data->options->array[0].value;

  // covers both 0 and different id
  ERROR_INTERACTION((player.scurry_id != player.user_id), "You do not own a scurry!");

  ERROR_INTERACTION((player.acorns < SCURRY_CREATION_COST), 
      "You need more acorns to rename your scurry!");

  size_t input_len = strlen(input);
  ERROR_INTERACTION((input_len > 16 || input_len < 4), 
      "The name needs to be between 4 and 16 characters.");

  // check if the name is unique
  PGresult* check_scurry = (PGresult*) { 0 };

  check_scurry = SQL_query(check_scurry, "select * from public.scurry where s_name like '%s'", input);

  char error_msg[64] = { };
  u_snprintf(error_msg, sizeof(error_msg), "Sorry, the name \"%s\" is already taken!", input);
  ERROR_DATABASE_RET((PQntuples(check_scurry) == 1), error_msg, check_scurry);

  PQclear(check_scurry);

  PGresult* rename_scurry = (PGresult*) { 0 };

  rename_scurry = SQL_query(rename_scurry, "update public.scurry set s_name = '%s' where owner_id = %ld", input, player.scurry_id);
  PQclear(rename_scurry);

  struct sd_header_params header = { 0 };
  struct sd_rename_scurry params = { 0 };

  // the only embed to ever send from this command is a successful creation
  header.embed = (struct discord_embed) 
  {
    .color = ACTION_SUCCESS,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Scurry successfully renamed!"),
    .description = u_snprintf(params.description, sizeof(params.description),
        "Your scurry has been renamed to **%s**. Check it out with the command `/scurry_info`!", input),
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), 
          "/scurry_help | For info about scurries!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url),
          GIT_PATH, item_types[TYPE_NO_ACORNS].file_path)
    }
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