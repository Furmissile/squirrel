
struct sd_create_scurry 
{
  char description[256];
  char footer_text[64];
  char footer_url[128];
};

/* Listens for slash command interactions */
int create_interaction(const struct discord_interaction *event) 
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  char* input = event->data->options->array[0].value;

  /* Check to make sure the player isnt in a scurry, the input is right, and the player has enough acorns */
  ERROR_INTERACTION((player.scurry_id > 0), 
      "You are already in a scurry! Try leaving your current scurry.");

  ERROR_INTERACTION((player.conjured_acorns < SCURRY_CREATION_COST), 
      "You need more conjured acorns to create a scurry!");

  size_t input_len = strlen(input);
  ERROR_INTERACTION((input_len > 16 || input_len < 4), 
      "The name needs to be between 4 and 16 characters.");

  // check if the name is unique
  PGresult* check_scurry = (PGresult*) { 0 };

  check_scurry = SQL_query(check_scurry, "select * from public.scurry where name like '%s'", input);

  char error_msg[64] = { };
  u_snprintf(error_msg, sizeof(error_msg), "Sorry, the name \"%s\" is already taken!", input);
  DATABASE_ERROR((PQntuples(check_scurry) == 1), error_msg, check_scurry);

  PQclear(check_scurry);

  check_scurry = SQL_query(check_scurry, "INSERT INTO public.scurry VALUES(%ld, '%s', 4, 0, 0)", 
      player.user_id, input);
  PQclear(check_scurry);

  // check if input was successful
  check_scurry = SQL_query(check_scurry, "select * from public.scurry where owner_id = %ld", player.user_id);

  DATABASE_ERROR((PQntuples(check_scurry) == 0), 
      "An error has occurred while creating this scurry. \n"
      "Please report this error to the support server! (link on last page of player help)", check_scurry);
  PQclear(check_scurry);

  player.scurry_id = player.user_id; // the player is the owner
  player.conjured_acorns -= SCURRY_CREATION_COST;

  struct sd_header_params header = { 0 };
  struct sd_create_scurry params = { 0 };

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
    .title = u_snprintf(header.title, sizeof(header.title), "Scurry successfully created!"),
    .description = u_snprintf(params.description, sizeof(params.description),
        "Your scurry is named **%s**. Check it out with the command `Info > Scurry Info`!", input),
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), 
          "Info > Scurry Help | For info about scurries!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url),
          GIT_PATH, misc[MISC_HELP].file_path)
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

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);

  return 0;
}