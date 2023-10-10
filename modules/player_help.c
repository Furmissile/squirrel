int player_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event);

  struct sd_help_info params = { 0 };

  int page_num = (player.button_idx != ERROR_STATUS) ? player.button_idx +1 : 1;

  params.fields = calloc(1, sizeof(struct discord_embed_field));
  params.field_names = calloc(1, sizeof(*params.field_names));
  params.field_values = calloc(1, sizeof(*params.field_values));

  switch (page_num -1)
  {
    case P_TOPIC_INTRO:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Bot Interaction"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" Never heard of slash commands? No need to fret! \n"
            " "BULLET" Enter "FORAGE_ID" to get started with an adventure and press any button. \n"
            " "BULLET" After pressing `/`, click the squirrel in the left column of the menu and the description for each command should come up. \n"
            " "BULLET" "ACORNS" *Acorn* is the main currency and "ACORN_COUNT" *acorn count* is your score. \n"
            " "BULLET" "GOLDEN_ACORNS" *Golden acorns* can be spent on increasing your score and healing! \n"
            " "BULLET" The "PROFICIENCY_ACORN" *Proficiency acorn* and "LUCK_ACORN" *luck acorn* can be found while foraging tos boost your acorn or golden acorn earnings respectively!")
      };
      break;
    case P_TOPIC_STORY:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Unlocking the Story"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" Every encounter you complete has a chance to unlock an element of the biome's story! \n"
            " "BULLET" These elements are then added to "BIOME_STORY_ID". \n"
            " "BULLET" The option selected in the encounter does not effect the chance of getting element so long as the player doesn't die from the encounter.")
      };
      break;
    case P_TOPIC_PROGRESSION:
      APPLY_NUM_STR(b_interval, BIOME_INTERVAL);
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Biome Progression"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" Every **%s** "ACORN_COUNT" *acorn count*, you will enter the next biome. Once you get past the last biome, it cycles back. "
                "Send "INFO_ID" to see what biome you're in! \n"
            " "BULLET" The required acorn count to reach the next biome is also shown after your acorn count. \n"
            " "BULLET" With each biome, earnings and the damage dealt by encounters increase. \n"
            " "BULLET" Upon losing all your "HEALTH" *health*, your acorn count is set back! \n"
            " "BULLET" If you get past the first biome cycle in a run before you die, your acorn count is *halved*. Otherwise, it is set back to **0**.",
            b_interval)
      };
      break;
    case P_TOPIC_SQUIRREL:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Squirrels"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" With certain "ACORN_COUNT" *acorn count* milestones, you can unlock new squirrels! \n"
            " "BULLET" Each squirrel has its own benefit based on player needs. Send "SQUIRRELS_ID" to check it out!")
      };
      break;
    case P_TOPIC_CHANGE_COLOR:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Changing Message Color"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" The color of your embed can be changed by inputing a hex value into "COLOR_ID". \n"
            " "BULLET" Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!")
      };
      break;
    case P_TOPIC_STEAL:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Stealing Player Acorns"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" Stealing from other players gives "ACORNS" *acorns* and "GOLDEN_ACORNS" *golden acorns* using "STEAL_ID". \n"
            " "BULLET" Stolen rewards increase when you upgrade your "PROFICIENCY_ICON" *Proficiency* and "LUCK_ICON" *Luck* stat respectively.")
      };
      break;
    case P_TOPIC_CONJURED_ACORNS:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Conjured Acorns"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" "CONJURED_ACORNS" *Conjured acorns* are fragments of dark power originating from the "LAST_ACORN_ICON" *Last Acorn*. \n"
            " "BULLET" They can be found in corruption-touched areas including: \n" 
                " "INDENT" ("GRASSLANDS_ICON") *Witch Swamp*, \n" 
                " "INDENT" ("SEEPING_SANDS_ICON") *Wormhole*, \n"
                " "INDENT" ("NATURE_END_ICON") *Breached Woods*, \n"
                " "INDENT" ("DEATH_GRIP_ICON") *Necrotic Lakes*, \n"
                " "INDENT" ("LAST_ACORN_ICON") *Death's Locus* \n"
            " "BULLET" Conjured acorns can be collected to super charge your squirrel and boost its effect! Players cannot swap squirrels while super charge is active. \n")
      };
      break;
    case P_TOPIC_SCURRY:
      APPLY_NUM_STR(scurry_cost, SCURRY_CREATION_COST);
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Scurries"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" Scurries can be created with **%s** "ACORNS" *acorns*. A player can not own more than **1** scurry. \n"
            " "BULLET" If you are looking to transfer ownership of your scurry, reset it, or disband it altogether, please reach out to the [support server](https://discord.gg/Dd8Te3HmPW). \n"
            " "BULLET" For more info on scurries, send "SCURRY_HELP_ID"! \n"
            " "BULLET" Already in a scurry? Send "SCURRY_INFO_ID"!",
            scurry_cost)
      };
      break;
    case P_TOPIC_HELP:
      *params.fields = (struct discord_embed_field) {
        .name = u_snprintf(*params.field_names, sizeof(*params.field_names), 
            ""ACORNS" Reporting Issues"),
        .value = u_snprintf(*params.field_values, sizeof(*params.field_values),
            " "BULLET" If you've found a bug or just have a question that wasn't addressed, feel free to join the [support server](https://discord.gg/Dd8Te3HmPW)! \n"
            " "BULLET" If you encounter a bug while foraging, please screenshot the message immediately before reporting it! Otherwise, the bug may not be handled. \n"
            " "BULLET" Please refer to the [license](https://github.com/Furmissile/squirrel/blob/main/LICENSE) if you wish to use this project. If unauthorized copies are encountered, please immediately report it to <#1047233819201261748> with the supporting link!")
      };
  }

  init_help_buttons(event, &params, page_num -1, TYPE_PLAYER_HELP, P_TOPIC_SIZE -1);

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Player Help (%d of %d)", 
        page_num, P_TOPIC_SIZE),

    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 1
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ACORNS].file_path)
    }
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 4
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
        .array = &action_rows,
        .size = 1
      }
    }

  };

  char values[16384];
  CCORDcode code = discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  free(params.field_names);
  free(params.field_values);
  free(params.fields);

  return 0;
}