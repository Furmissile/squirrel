/* Checks user input for hex value accuracy */
char* is_color(const struct discord_interaction *event, char* input)
{
  if (strlen(input) != 6) {
    return NULL;
  } 
  else {
  for (int i = 0; i < 6; i++)
    switch (input[i])
    {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        continue;
      default:
        error_message(event, "Invalid input. Please try again!");
        return NULL;
    }
  }

  player.color = (int)strtol(input, NULL, 16);
  player.acorns -= COLOR_COST;

  return input;
}

/* Listens for slash command interactions */
int color_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);

  char* input = event->data->options->array[0].value;

  ERROR_INTERACTION((player.acorns < COLOR_COST), 
      format_str(SIZEOF_DESCRIPTION, 
          "This action requires %d acorns. You need %d more acorns!", 
          COLOR_COST, COLOR_COST - player.acorns) );

  ERROR_INTERACTION( ( !is_color(event, lowercase(input)) ), "Invalid input. Please try again!");

  discord_msg->embed->title = format_str(SIZEOF_TITLE, "Color successfully changed!");
  discord_msg->embed->color = player.color;

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg);

  update_player_row(player);

  return 0;
}