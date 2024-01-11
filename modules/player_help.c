int player_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  ERROR_INTERACTION((event->message->timestamp /1000 < player.timestamp), 
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp /1000;

  struct sd_help_info params = { 0 };

  int page_num = (player.button_idx != ERROR_STATUS) ? player.button_idx +1 : 1;

  switch (page_num -1)
  {
    case P_TOPIC_INTRO:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Bot Controls"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Enter "START_ID" and press \"Forage\" to get started with an adventure. \n"
            " "BULLET" Buttons I, II, and III correspond to stashes I, II, and III. \n"
            " "BULLET" \"rotate\" will rotate your current piece 90 degrees *clockwise*. \n"
            " "BULLET" The objective of the game is to complete as many stashes as you can before you run out of space! \n"
            " "BULLET" Looking to get another player's game stats? View their profile and under \"Apps\", press on \"Player\"!")
      };
      break;
    case P_TOPIC_PROGRESSION:
      APPLY_NUM_STR(b_interval, BIOME_INTERVAL);
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Gameplay"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Upon completing a stash, all the acorns' values are summed up and added to your score or "ACORN_COUNT" *acorn count*. \n"
            " "BULLET" Every **%s** "ACORN_COUNT" *acorn count*, you will enter the next biome. Once you get through the last biome, it cycles back. "
                "Go to `Info` to see what biome you're in! \n"
            " "BULLET" Encounters have a chance to occur each time you add a piece to one of your stashes. \n"
            " "BULLET" If you get an encounter, your incoming piece is swapped out regardless of what pieces you've already placed. \n"
            " "BULLET" A "QUEST_MARKER" *quest marker* appears following \"Current\" to signal your incoming piece was swapped.",
            b_interval)
      };
      break;
    case P_TOPIC_STORY:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Unlocking the Story"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Every encounter you complete has a chance to unlock an element of the biome's story! \n"
            " "BULLET" These elements are then added to your known biome story found at `Info > Statistics > Biome Story`.")
      };
      break;
    case P_TOPIC_SQUIRREL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Squirrels"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Squirrels can be purchased with "CONJURED_ACORNS" *conjured acorns*! \n"
            " "BULLET" Squirrels are cosmetic and themed by in-game seasons. Check squirrels out at `Squirrels`!")
      };
      break;
    case P_TOPIC_CHANGE_COLOR:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Changing Message Color"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" The color of your embed can be changed by inputting a hex value into "COLOR_ID". \n"
            " "BULLET" Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!")
      };
      break;
    case P_TOPIC_STEAL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Stealing Player Acorns"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Every player starts with a half-filled "TREASURE" *treasury*. Check it out at `Stealing`! \n"
            " "BULLET" Steal from other players' treasuries to fill your own for extra benefits!")
      };
      break;
    case P_TOPIC_SCURRY:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Scurries"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Players can join or create scurries for co-op fun! \n"
            " "BULLET" Scurries cost **%s** "CONJURED_ACORNS" *conjured acorns* to create. A player can not own more than **1** scurry. \n"
            " "BULLET" For more info on scurries, press on the \"Scurry\" button! \n"
            " "BULLET" Already in a scurry? Go to `Scurry Info`!",
            SCURRY_CREATION_COST)
      };
      break;
    case P_TOPIC_DAILY:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Daily Milestones"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Complete stashes everyday to reach the daily milestones and earn extra "CONJURED_ACORNS" *conjured acorns*. \n"
            " "BULLET" There are 3 milestones: **%d**, **%d**, and **%d** completed stashes. Can you reach them all? \n"
            " "BULLET" Check out how far you have made it at `Info`!",
            FIRST_DAILY, SECOND_DAILY, THIRD_DAILY)
      };
      break;
    case P_TOPIC_HELP:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Reporting Issues"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" If you've found a bug or have a question that wasn't addressed, feel free to join the [support server](https://discord.gg/Dd8Te3HmPW)! \n"
            " "BULLET" If you encounter a bug, please screenshot the message immediately before reporting it! Otherwise, the bug may not be handled. \n"
            " "BULLET" Please refer to the [license](https://github.com/Furmissile/squirrel/blob/main/LICENSE) if you wish to use this project. "
                "If unauthorized copies are encountered, please immediately report it to <#1047233819201261748> with the supporting link!")
      };
  }

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
    .title = u_snprintf(header.title, sizeof(header.title), "Player Help (%d of %d)", 
        page_num, P_TOPIC_SIZE),

    .fields = &(struct discord_embed_fields) {
      .array = &params.field,
      .size = 1
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, slice_types[TYPE_ACORNS].item.file_path)
    }
  };

  struct sd_pages pages = { 0 };

  init_page_buttons(event, &pages, page_num -1, TYPE_PLAYER_HELP, P_TOPIC_SIZE -1);

  struct sd_help_buttons help = { 0 };

  init_help_buttons(event, &help, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = pages.buttons,
        .size = sizeof(pages.buttons)/sizeof(*pages.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = help.buttons,
        .size = sizeof(help.buttons)/sizeof(*help.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = util_data.buttons_displayed
      }
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = action_rows,
        .size = 3
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);

  return 0;
}