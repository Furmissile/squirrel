int event_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_help_info params = { 0 };

  int page_num = (player.button_idx != ERROR_STATUS) ? player.button_idx +1 : 1;

  switch (page_num -1)
  {
    case E_TOPIC_SEASONS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Acorn Count Event"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" This is an ongoing event where players compete based on "ACORN_COUNT" *acorn count*. \n"
            " "BULLET" Acorn count is unaffected by your stats and buffs, so be sure to have your luck in your right pocket! \n"
            " "BULLET" You can increase your score by foraging and finding the "PROFICIENCY_ACORN" *proficiency acorn*. \n")
      };
      break;
    case E_TOPIC_ACORN_COUNT:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Seasons"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Seasons passively change every week. Each season gives a different boost to "ACORNS" *acorn* earnings. \n"
            " "BULLET" Seasons include Spring (+**20**%%), Summer (+**10**%%), Fall (+**5**%%), and Winter (No boost). \n"
            " "BULLET" Check your info to find out what season is active and when it ends! \n")
      };
      break;
    case E_TOPIC_SPRING:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Spring Chicken Event (Spring)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Trees everywhere are dropping "GOLDEN_ACORNS" *golden acorns*! \n"
            " "BULLET" Lasts throughout the in-game Spring season. \n"
            " "BULLET" "GOLDEN_ACORNS" *Golden acorns* have a chance to be included in every forage instead of just "
                "from stealing or "LOST_STASH" *lost stashes*! \n")
      };
      break;
    case E_TOPIC_SUMMER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Garden Raid Event (Summer)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Everyone is growing all sorts of useful victuals! *RaId ThEiR gArDeNsSs!* \n"
            " "BULLET" Lasts throughout the in-game Summer season. \n"
            " "BULLET" You will find all sorts of victuals including "BLUEBERRY_VICTUALS" *blueberries*, "SEED_VICTUALS" *seeds*, and "CHERRY_VICTUALS" *cherries* for bonus rewards! \n"
            " "BULLET" Victuals are __NOT__ resources. They are not added to your inventory! What they give follows in parentheses.")
      };
      break;
    case E_TOPIC_FALL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Hibernation Event (Fall)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Winter is coming! Take this time to snatch all the "ACORNS" *acorns* you can before hibernation! \n"
            " "BULLET" Lasts throughout the in-game Fall season. \n"
            " "BULLET" Energy regeneration is halved! \n")
      };
      break;
    case E_TOPIC_WINTER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Bunny's Endeavor (Winter)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Resources are depleting! Bunny is looking for able-bodies who can fetch him "CATNIP" *catnip*. He'll pay handsomely for it too. \n"
            " "BULLET" Lasts throughout the in-game Winter season! Check it out at `Info > Bunny's Wares` (only appears while the event is active)! \n"
            " "BULLET" Catnip has a chance to be included with every forage! \n")
      };
      break;
    case E_TOPIC_CHRISTMAS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Christmas Nutcracker (December)"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" "RIBBONED_ACORN" *Ribboned acorns* are hidden throughout the world by people as a gift for critters to take! \n"
            " "BULLET" Not all were friendly, however, because some might give you "COAL" *coal*! \n"
            " "BULLET" Lasts throughout December! \n")
      };
      break;
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

  struct sd_pages pages = { 0 };

  init_page_buttons(event, &pages, page_num -1, TYPE_EVENT_HELP, E_TOPIC_SIZE -1);

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
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
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

  char values[16384];
  CCORDcode code = discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}