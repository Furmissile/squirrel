/*

  This file handles the help embed.

  - Displays general concepts of the game.

*/


void p_help(const struct discord_interaction *event, struct sd_message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  embed->title = format_str(SIZEOF_TITLE, "Player Help (%d of %d)", page_num, P_TOPIC_SIZE);

  embed->color = player.color;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help? Check out the info below to see if you're question can be answered! \n"
      "Need help with purchases? Request the respective embed for details! \n\n");

  struct discord_embed_field *player_help_pages = (struct discord_embed_field[])
  {
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Bot Interaction"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Never heard of slash commands? Don't fret! \n"
          " "OFF_ARROW" Enter `/forage` to get started with an adventure and press any button. \n"
          " "OFF_ARROW" If you're feeling adventurous, click the squirrel in the left column of the menu and the description for each command should come up. \n"
          " "OFF_ARROW" "ACORNS" Acorn is the main currency and "ACORN_COUNT" acorn count is your score in the game.")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Biome Progression"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Every **%s** "ACORN_COUNT" acorn count, you will enter the next biome. Once you get past the last biome, it cycles back. Send `/info` to check it out! \n"
          " "OFF_ARROW" The required acorn count to reach the next biome is also shown after your acorn count. \n"
          " "OFF_ARROW" With each biome, the damage dealt by encounters and earnings increases. \n"
          " "OFF_ARROW" Upon losing all your "HEALTH" health, your acorn count is set back! \n"
          " "OFF_ARROW" If you get past the first biome cycle in a run and die, your acorn count is halved. Otherwise, it is set back to 0.",
          num_str(BIOME_INTERVAL), num_str(PRESTIGE_REQ))
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Squirrels"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" With certain acorn count milestones, you can unlock new squirrels! \n"
          " "OFF_ARROW" Each squirrel has its own benefit based on player needs. Send `/squirrels` to check it out! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Changing Message Color"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" The color of your embed can be changed by inputing a hex value into `/color`. \n"
          " "OFF_ARROW" Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Stealing Player Acorns"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Stealing from other players gives "ACORNS" acorns and "GOLDEN_ACORNS" golden acorns. \n"
          " "OFF_ARROW" Golden acorns can be spent on buffs to make the most of your run! Check it out using `/buffs`!")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Scurries"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Scurries can be created with **%s** acorns. A player can not own more than 1 scurry. \n"
          " "OFF_ARROW" If you are looking to transfer ownership of your scurry, reset it, or disban it altogether, please reach out to the support server (link on the last page). \n"
          " "OFF_ARROW" For more info on scurries, send `/scurry_help`! \n"
          " "OFF_ARROW" Already in a scurry? Send `/scurry_info`!",
          num_str(SCURRY_CREATION_COST))
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Conjured Acorns"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" "CONJURED_ACORNS" Conjured Acorns are fragments of dark power originating from the **Last Acorn**. They can be found in corruption-touched areas throughout the world. \n"
          " "OFF_ARROW" Corruption-touched areas include: \n"INDENT" "GRASSLANDS_ICON" *Witch Swamp* \n"INDENT" "SEEPING_SANDS_ICON" *Wormhole* \n"INDENT" "NATURE_END_ICON" *Breached Woods* \n"INDENT" "DEATH_GRIP_ICON" *Necrotic Lakes* \n"INDENT" "LAST_ACORN_ICON" *Death's Locus* \n"
          " "OFF_ARROW" Conjured acorns are used to boost your current squirrel's effect and are transferrable among squirrels. \n"
          " "OFF_ARROW" Check it out in `/buffs`!")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Reporting Issues"),
      .value = format_str(SIZEOF_FIELD_VALUE, 
          " "OFF_ARROW" If you've found a bug or just have a question that wasn't addressed, feel free to join the [support server](https://discord.gg/Dd8Te3HmPW)! \n")
    }
  };

  discord_msg->buttons = build_help_buttons(event, page_num -1, TYPE_PLAYER_HELP, P_TOPIC_SIZE -1);

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = 1;
  embed->fields->array = calloc(1, sizeof(struct discord_embed_field));
  embed->fields->array[0] = player_help_pages[page_num -1];
  
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Happy Foraging!");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORNS].item.file_path);
}


/* Listens for slash command interactions */
int player_help_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  p_help(event, discord_msg);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = discord_msg->buttons
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg->buttons);
  free(discord_msg);

  return 0;
}