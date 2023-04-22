/*

  This file handles the help embed.

  - Displays general concepts of the game.

*/

enum P_TOPIC {
  TOPIC_INTRO,
  TOPIC_PROGRESSION,
  TOPIC_SQUIRREL,
  TOPIC_CHANGE_COLOR,
  TOPIC_STEAL,
  TOPIC_SCURRY,
  TOPIC_HELP,
  TOPIC_SIZE
};

void p_help(struct sd_message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Player Help");

  embed->color = player.color;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help? Check out the info below to see if you're question can be answered! \n"
      "Need help with purchases? Request the respective embed for details!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = TOPIC_SIZE;
  embed->fields->array = calloc(TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[TOPIC_INTRO].name = format_str(SIZEOF_TITLE, ""ACORNS" Bot Interaction");
  embed->fields->array[TOPIC_INTRO].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Enter `/forage` to get started with an adventure and press any button. \n"
      " "OFF_ARROW" What each command does will pop up just below the command after inputing `/`. \n"
      " "OFF_ARROW" "ACORNS" Acorn is the main currency.");

  embed->fields->array[TOPIC_PROGRESSION].name = format_str(SIZEOF_TITLE, ""ACORNS" Biome Progression");
  embed->fields->array[TOPIC_PROGRESSION].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" You must reach the next designated value of acorn count to proceed to the next biome (every **%s** acorns). \n"
      " "OFF_ARROW" With each biome, the damage in encounters and reward increases. Send `/info` to check it out! \n"
      " "OFF_ARROW" Upon losing all your "HEALTH" health, your acorn count is set back! \n"
      " "OFF_ARROW" If your score was above **%s** "ACORN_COUNT" acorn count, you will start with half your total score.",
      num_str(BIOME_INTERVAL), num_str(PRESTIGE_REQ));

  embed->fields->array[TOPIC_SQUIRREL].name = format_str(SIZEOF_TITLE, ""ACORNS" Squirrels");
  embed->fields->array[TOPIC_SQUIRREL].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" With certain acorn count milestones, you can unlock new squirrels! \n"
      " "OFF_ARROW" Each squirrel has its own benefit based on player needs. Send `/squirrels` to check it out! \n");

  embed->fields->array[TOPIC_CHANGE_COLOR].name = format_str(SIZEOF_TITLE, ""ACORNS" Changing Message Color");
  embed->fields->array[TOPIC_CHANGE_COLOR].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" The color of your embed can be changed by inputing a hex value into `/color`. \n"
      " "OFF_ARROW" Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!");

  embed->fields->array[TOPIC_SCURRY].name = format_str(SIZEOF_TITLE, ""ACORNS" Scurries");
  embed->fields->array[TOPIC_SCURRY].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Scurries can be created with **%s** acorns. A player can not own more than 1 scurry. \n"
      " "OFF_ARROW" For more info on scurries, send `/scurry_help`! \n"
      " "OFF_ARROW" In a scurry? send `/scurry_info`!",
      num_str(SCURRY_CREATION_COST));

  embed->fields->array[TOPIC_STEAL].name = format_str(SIZEOF_TITLE, ""ACORNS" Stealing Player Acorns");
  embed->fields->array[TOPIC_STEAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Stealing from other players gives acorns and "GOLDEN_ACORNS" golden acorns. \n"
      " "OFF_ARROW" Golden acorns can be spent on buffs! Check it out using `/buffs`.");

  embed->fields->array[TOPIC_HELP].name = format_str(SIZEOF_TITLE, ""ACORNS" Reporting Issues");
  embed->fields->array[TOPIC_HELP].value = format_str(SIZEOF_FIELD_VALUE, 
      " "OFF_ARROW" If you've found a bug or just have a question that wasn't addressed, feel free to join the [support server](https://discord.gg/Dd8Te3HmPW)! \n");
  
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Happy Foraging!");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORNS].item.file_path);
}


/* Listens for slash command interactions */
int help_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  p_help(discord_msg);

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

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

  free(discord_msg);

  return 0;
}