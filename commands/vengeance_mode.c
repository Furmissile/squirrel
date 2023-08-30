int vengeance_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id); 

  char msg_content[256] = { };

  if (player.acorn_count < BIOME_INTERVAL * BIOME_SIZE)
  {
    u_snprintf(msg_content, sizeof(msg_content), "You do not meet the requirements to enable Vengeance Mode! You can find the requirements in </player_help:1089663968253067264>.");
  }
  else if (player.vengeance_flag)
  {
    player.vengeance_flag = 0;
    u_snprintf(msg_content, sizeof(msg_content), "You have "HELP_MARKER" **disabled** Vengeance Mode!");
  }
  else
  {
    player.vengeance_flag = 1;
    u_snprintf(msg_content, sizeof(msg_content), "You have "QUEST_MARKER" **enabled** Vengeance Mode!");
  }

  discord_create_interaction_response(client, event->id, event->token, 
  &(struct discord_interaction_response)
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = msg_content
    }
  },
  NULL);

  update_player_row(&player);

  return 0;
}
