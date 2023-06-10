int event_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  struct sd_help_info params = { 0 };

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  switch (page_num -1)
  {
    case E_TOPIC_SEASONS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Acorn Count Event"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" This is an ongoing event where players compete based on "ACORN_COUNT" *acorn count*. \n"
            " "OFF_ARROW" Acorn count is uneffected by your stats and buffs, so be sure to have your luck in your right pocket! \n"
            " "OFF_ARROW" You can increase your score by </forage:1089663881959460926>, successful steals, and applying the "PROFICIENCY_ACORN" *proficiency acorn*. \n")
      };
      break;
    case E_TOPIC_ACORN_COUNT:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Seasons"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Seasons change every week and passively occur. Each season gives a different boost to "ACORNS" *acorns* earnings. \n"
            " "OFF_ARROW" Seasons include Spring (+**20**%%), Summer (+**10**%%), Fall (+**5**%%), and Winter (No boost). \n"
            " "OFF_ARROW" Send </season_info:1089664143419768972> to find out when the current season ends! \n")
      };
      break;
    case E_TOPIC_SPRING:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Spring Chicken Event (Spring)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Trees everywhere are dropping "GOLDEN_ACORNS" *golden acorns*! \n"
            " "OFF_ARROW" Lasts throughout the in-game Spring season. \n"
            " "OFF_ARROW" "GOLDEN_ACORNS" *Golden acorns* have a chance to be included in every </forage:1089663881959460926> instead of just "
                "from </steal:1089663966520819865> or "LOST_STASH" *lost stashes*! \n")
      };
      break;
    case E_TOPIC_SUMMER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Garden Raid Event (Summer)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Everyone is growing all sorts of useful victuals! *RaId ThEiR gArDeNsSs!* \n"
            " "OFF_ARROW" Lasts throughout the in-game Summer season. \n"
            " "OFF_ARROW" Look for all sorts of victuals including "BLUEBERRY_VICTUALS" *blueberries*, "SEED_VICTUALS" *seeds*, and "CHERRY_VICTUALS" *cherries* for bonus rewards! \n"
            " "OFF_ARROW" Victuals are __NOT__ resources. They are not added to your inventory! What they give appear in parentheses right after.")
      };
      break;
    case E_TOPIC_FALL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Hibernation Event (Fall)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Winter is coming! Take this time to snatch all the "ACORNS" *acorns* you can before hibernation! \n"
            " "OFF_ARROW" Lasts throughout the in-game Fall season. \n"
            " "OFF_ARROW" Energy regeneration for all players is halved! \n")
      };
      break;
    case E_TOPIC_WINTER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Bunny's Endeavor (Winter)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "OFF_ARROW" Resources are depleting! Bunny is looking for able-bodies who can fetch him "CATNIP" *catnip*. He'll pay handsomely for it too. \n"
            " "OFF_ARROW" Lasts throughout the in-game Winter season! Check it out with </bunny_endeavor:1089664142203424769>! \n"
            " "OFF_ARROW" Catnip has a chance to be included in every </forage:1089663881959460926>! \n")
      };
      break;
  }

  init_help_buttons(event, &params, page_num -1, TYPE_EVENT_HELP, E_TOPIC_SIZE -1);

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
    .title = u_snprintf(header.title, sizeof(header.title), "Event Help (%d of %d)", 
        page_num, E_TOPIC_SIZE),

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