int event_help_interaction(const struct discord_interaction *event)
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
    case E_TOPIC_ACORN_COUNT:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Acorn Count Event"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" This is an ongoing event where players compete based on "ACORN_COUNT" *acorn count*. \n"
            " "BULLET" Check out the leaderboard at `Info > Statistics > Leaderboard`!")
      };
      break;
    case E_TOPIC_SEASONS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Seasons"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Seasons passively change every week. Each season offers a different material to collect that have a chance to appear on each piece. \n"
            " "BULLET" Each new season brings a higher quality material so get your foraging skills ready! \n"
            " "BULLET" Go to `Info` to find out what season it is and when it ends! \n")
      };
      break;
    case E_TOPIC_SPRING:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Spring's Seeds"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Spring is the first week of every month and "SEED_VICTUALS" *Seeds* are in abunance! \n"
            " "BULLET" Their abundance causes them to have the least value of all the seasonal rewards. \n")
      };
      break;
    case E_TOPIC_SUMMER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Summer's Cherries"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Summer is the second week of every month and everyone is getting their hands on "CHERRY_VICTUALS" *cherries*! \n"
            " "BULLET" They are often located in humans' gardens which can be tricky at times! This makes cherries a higher value than "SEED_VICTUALS" *Seeds*. \n")
      };
      break;
    case E_TOPIC_FALL:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Fall's Blueberries"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Fall is the third week of every month and every squirrel loves "BLUEBERRY_VICTUALS" *blueberries*! \n"
            " "BULLET" They are an absolute delight due to their variance in taste! This makes blueberries a higher value than "CHERRY_VICTUALS" *cherries*. \n")
      };
      break;
    case E_TOPIC_WINTER:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name),
            ""ACORNS" Winter's Catnip"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Winter lasts the remainder of the month and Bunny the cat is looking for able-bodies to fetch him "CATNIP" *catnip*! \n"
            " "BULLET" Bunny has friends in high places so he pays with an added bonus to "ACORN_COUNT" *acorn count*. This makes catnip a higher value than "BLUEBERRY_VICTUALS" *blueberries*. \n")
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
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, slice_types[TYPE_ACORNS].item.file_path)
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