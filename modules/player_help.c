int player_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  struct sd_help_info params = { 0 };

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  switch (page_num -1)
  {
    case P_TOPIC_INTRO:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Bot Interaction"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Never heard of slash commands? No need to fret! \n"
            " "OFF_ARROW" Enter </forage:1089663881959460926> to get started with an adventure and press any button. \n"
            " "OFF_ARROW" If you're feeling adventurous, click the squirrel in the left column of the menu and the description for each command should come up. \n"
            " "OFF_ARROW" "ACORNS" *Acorn* is the main currency and "ACORN_COUNT" *acorn count* is your score in the game."
            " "OFF_ARROW" "GOLDEN_ACORNS" *Golden acorns* can be spent on buffs to make the most of your run! Check it out using </buffs:1089663883876257833>!")
      };
      break;
    case P_TOPIC_PROGRESSION:
      APPLY_NUM_STR(b_interval, BIOME_INTERVAL);
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Biome Progression"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Every **%s** "ACORN_COUNT" *acorn count*, you will enter the next biome. Once you get past the last biome, it cycles back. "
                "Send </info:1089663884673167440> to see what biome you're in! \n"
            " "OFF_ARROW" The required acorn count to reach the next biome is also shown after your acorn count. \n"
            " "OFF_ARROW" With each biome, the damage dealt by encounters and earnings increases. \n"
            " "OFF_ARROW" Upon losing all your "HEALTH" *health*, your acorn count is set back! \n"
            " "OFF_ARROW" If you get past the first biome cycle in a run before you die, your acorn count is *halved*. Otherwise, it is set back to **0**.",
            b_interval)
      };
      break;
    case P_TOPIC_SQUIRREL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Squirrels"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" With certain "ACORN_COUNT" *acorn count* milestones, you can unlock new squirrels! \n"
            " "OFF_ARROW" Each squirrel has its own benefit based on player needs. Send </squirrels:1089663967460327525> to check it out!")
      };
      break;
    case P_TOPIC_CHANGE_COLOR:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Changing Message Color"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" The color of your embed can be changed by inputing a hex value into </color:1089663885692370954>. \n"
            " "OFF_ARROW" Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!")
      };
      break;
    case P_TOPIC_STEAL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Stealing Player Acorns"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Stealing from other players gives "ACORNS" *acorns* and "GOLDEN_ACORNS" *golden acorns* using </steal:1089663966520819865>. \n"
            " "OFF_ARROW" Stolen rewards can be improved when you upgrade your "PROFICIENCY_ICON" *Proficiency* and "LUCK_ICON" *Luck* stat respectively.")
      };
      break;
    case P_TOPIC_CONJURED_ACORNS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Conjured Acorns"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" "CONJURED_ACORNS" *Conjured acorns* are fragments of dark power originating from the "LAST_ACORN_ICON" **Last Acorn**. \n"
            " "OFF_ARROW" They can be found in corruption-touched areas including: \n" 
                " "INDENT" "GRASSLANDS_ICON" *Witch Swamp*, \n" 
                " "INDENT" "SEEPING_SANDS_ICON" *Wormhole*, \n"
                " "INDENT" "NATURE_END_ICON" *Breached Woods*, \n"
                " "INDENT" "DEATH_GRIP_ICON" *Necrotic Lakes*, \n"
                " "INDENT" "LAST_ACORN_ICON" *Death's Locus* \n"
            " "OFF_ARROW" Conjured acorns are used to boost your current squirrel's effect and are transferrable between squirrels. \n"
            " "OFF_ARROW" Check out the buff in </buffs:1089663883876257833>!")
      };
      break;
    case P_TOPIC_SCURRY:
      APPLY_NUM_STR(scurry_cost, SCURRY_CREATION_COST);
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Scurries"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value), 
            " "OFF_ARROW" Scurries can be created with **%s** "ACORNS" *acorns*. A player can not own more than **1** scurry. \n"
            " "OFF_ARROW" If you are looking to transfer ownership of your scurry, reset it, or disban it altogether, please reach out to the support server (link on the last page). \n"
            " "OFF_ARROW" For more info on scurries, send </scurry_help:1089663969721065592>! \n"
            " "OFF_ARROW" Already in a scurry? Send </scurry_info:1089664056320852000>!",
            scurry_cost)
      };
      break;
    case P_TOPIC_VENGENCE_MODE:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Vengence Mode"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Vengeance mode is a type of hardcore that sets your health to **1**, is unphased by your "STRENGTH_ICON" *strength* stat"
                " and the chance to find an encounter is also increased. \n"
            " "OFF_ARROW" In turn, you receive *double* the base value of "ACORNS" *acorn*, "ACORN_COUNT" *acorn count*, and "GOLDEN_ACORNS" *golden acorn* earnings! \n"
            " "OFF_ARROW" You must have been through the biome cycle at least once in your current run to be able to enable this mode! \n"
            " "OFF_ARROW" This mode can be enabled by the button in </info:1089663884673167440>! This mode is automatically disabled upon dying.")
      };
      break;
    case P_TOPIC_HELP:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Reporting Issues"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" If you've found a bug or just have a question that wasn't addressed, feel free to join the [support server](https://discord.gg/Dd8Te3HmPW)! \n")
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
      .array = &params.field,
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

  return 0;
}