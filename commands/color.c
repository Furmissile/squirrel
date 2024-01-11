/* Checks user input for hex value accuracy */
int is_color(char* input, struct sd_player *player)
{
  if (strlen(input) != 6) {
    return ERROR_STATUS;
  } 
  else {
    for (int i = 0; i < 6; i++)
      switch (input[i])
      {
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          continue;
        default:
          return ERROR_STATUS;
      }
  }

  player->color = (int)strtol(input, NULL, 16);

  return 0;
}

int color_interaction(const struct discord_interaction *event) 
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);
  
  char* input = event->data->options->array[0].value;
  char formatted_input[64] = { }; 
  lowercase(formatted_input, sizeof(formatted_input), input);

  ERROR_INTERACTION( ( is_color(formatted_input, &player) == ERROR_STATUS ), "Invalid input. Please try again!");

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },

    .title = u_snprintf(header.title, sizeof(header.title), "Color successfully changed!")
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