struct sd_squirrel_info 
{
  struct discord_component buttons[2];
  char custom_ids[2][64];

  struct discord_emoji emojis[2];
  char emoji_names[2][64];

  struct discord_embed_field field;
  char field_name[64];
  char field_value[1024];
};

void build_general_info(struct sd_squirrel_info *params, struct sd_player *player, struct sd_scurry *scurry, struct sd_pie_game *game)
{
  APPLY_NUM_STR(acorn_count, game->score);
  APPLY_NUM_STR(req_acorn_count, BIOME_INTERVAL * (player->biome_num +1) );
  APPLY_NUM_STR(conjured_acorns, player->conjured_acorns);

  struct sd_file_data biome_icon = biomes[player->biome].biome_icon;
  struct sd_file_data squirrel = squirrels[player->squirrel].squirrel;

  params->field.name = u_snprintf(params->field_name, sizeof(params->field_name), "General Info");

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char* seasons[4] = {"Spring", "Summer", "Fall", "Winter"};

  struct tm *info = get_UTC();
  char* season = seasons[(info->tm_mday/7 < 4) ? info->tm_mday/7 : 3];
  char* month = months[(info->tm_mday < 21) ? info->tm_mon : (info->tm_mon +1 == 12) ? 0 : info->tm_mon +1];
  int end_day = (info->tm_mday < 7) ? 7 : (info->tm_mday < 14) ? 14 : (info->tm_mday < 21) ? 21 : 1;

  // end_day can only be 1, otherwise it's greater than 3
  char* suffix = (end_day == 1) ? "st" : "th";

  u_snprintf(params->field_value, sizeof(params->field_value), " "INDENT" "ACORN_COUNT" Acorn Count: **%s**/%s",
      acorn_count, req_acorn_count);

  if (player->high_acorn_count > 0)
  {
    APPLY_NUM_STR(high_acorn_count, player->high_acorn_count);
    u_snprintf(params->field_value, sizeof(params->field_value), (" ("LEADER" **%s**)"),
        high_acorn_count);
  }

  u_snprintf(params->field_value, sizeof(params->field_value), 
      "\n "INDENT" <:%s:%ld> Biome: **%s**"
      "\n "INDENT" <:%s:%ld> Squirrel: **%s**"
      "\n "INDENT" "CONJURED_ACORNS" Conjured Acorns: **%s**"
      "\n "INDENT" "WEATHER" Season: **%s** (Ends on %s **%d%s**)"
      "\n "INDENT" "STAHR" Stashes Complete: **%d**",
      biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name,
      squirrel.emoji_name, squirrel.emoji_id, squirrel.formal_name,
      conjured_acorns,
      season, month, end_day, suffix,
      player->pies_complete);

  if (player->pies_complete < THIRD_DAILY)
    u_snprintf(params->field_value, sizeof(params->field_value), "/%d (**%d** hours left)",
      (player->pies_complete < FIRST_DAILY) ? FIRST_DAILY :
      (player->pies_complete < SECOND_DAILY) ? SECOND_DAILY : THIRD_DAILY,
      24 - info->tm_hour);

  if (player->scurry_id > 0)
  {
    u_snprintf(params->field_value, sizeof(params->field_value),
        "\n "INDENT" "GUILD_ICON" Scurry: **%s** \n", scurry->scurry_name);
  }

  params->field.value = params->field_value;
}

void p_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  struct sd_player player = { 0 };
  load_player_struct(&player, user->id, event->data->custom_id);

  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_pie_game game = { 0 };
  load_game_struct(&game, &player, player.user_id);

  struct sd_header_params header = { 0 };
  struct sd_squirrel_info params = { 0 };

  build_general_info(&params, &player, &scurry, &game);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          user->id, user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Player Info"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, 
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = &params.field,
      .size = 1
    }
  };

  int button_size = 1;

  params.buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .custom_id = u_snprintf(params.custom_ids[0], sizeof(params.custom_ids[0]), "%c0*%ld*.%ld",
      TYPE_INFO, user->id, event->member->user->id),
    .label = "Refresh"
  };

  if (player.scurry_id > 0)
  {
    params.emojis[0] = (struct discord_emoji)
    {
      .name = u_snprintf(params.emoji_names[0], sizeof(params.emoji_names[0]), 
              misc[MISC_SCURRY_ICON].emoji_name),
      .id = misc[MISC_SCURRY_ICON].emoji_id
    };

    params.buttons[1] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_SECONDARY,
      .custom_id = u_snprintf(params.custom_ids[1], sizeof(params.custom_ids[1]), "%c5*%ld*.%ld",
          TYPE_SCURRY_INFO, player.scurry_id, event->member->user->id),
      .label = "Scurry Info",
      .emoji = &params.emojis[0]
    };

    button_size++;
  }

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = button_size
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
      .components = &(struct discord_components)
      {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  struct sd_player event_player = { 0 };
  load_player_struct(&event_player, event->member->user->id, event->data->custom_id);
  update_player_row(&event_player, BASE_CD);
}

// only sent when NOT requesting another user
int info_from_buttons(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_pie_game game = { 0 };
  load_game_struct(&game, &player, player.user_id);

  struct sd_header_params header = { 0 };
  struct sd_squirrel_info params = { 0 };

  ERROR_INTERACTION((event->message->timestamp /1000 < player.timestamp), 
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp /1000;

  if (APPLICATION_ID == MAIN_BOT_ID)
    ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready! Please wait %d second(s).", BASE_CD);

  player.main_cd = time(NULL) + BASE_CD;

  build_general_info(&params, &player, &scurry, &game);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Player Info"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, 
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = &params.field,
      .size = 1
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct sd_statistics stats = { 0 };

  init_statistics_buttons(event, &stats, &player);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = util_data.buttons_displayed
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = stats.buttons,
        .size = sizeof(stats.buttons)/sizeof(*stats.buttons)
      }
    }
  };

  if (event->data->custom_id[0] == TYPE_SCURRY_LEAVE)
  {
    discord_edit_message(client, event->channel_id, event->message->id,
      &(struct discord_edit_message)
      {
        .embeds = &(struct discord_embeds) 
        {
          .array = &header.embed,
          .size = 1
        },
        .components = &(struct discord_components) {
          .array = action_rows,
          .size = 2
        }
      },
      NULL);
  }
  else {
    struct discord_interaction_response interaction = 
    {
      .type = DISCORD_INTERACTION_UPDATE_MESSAGE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .embeds = &(struct discord_embeds) 
        {
          .array = &header.embed,
          .size = 1
        },
        .components = &(struct discord_components) {
          .array = action_rows,
          .size = 2
        }
      }

    };

    discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
  }

  update_player_row(&player, BASE_CD);

  return 0;
}

int info_interaction(const struct discord_interaction *event)
{
  struct discord_ret_user ret_user = {
    .done = &p_info,
    .fail = &not_user,
    .keep = event
  };

  unsigned long user_id;
  if (event->data->target_id > 0) 
  {
    user_id = event->data->target_id;
    PGresult* search_player = (PGresult*) { 0 };
    search_player = SQL_query(search_player, "select * from public.player where user_id = %ld", user_id);

    DATABASE_ERROR((PQntuples(search_player) == 0), "This player does not exist!", search_player);
    PQclear(search_player);
  }
  else if (event->data->custom_id)
  {
    char user_id_buffer[64] = { };
    trim_buffer(user_id_buffer, sizeof(user_id_buffer), event->data->custom_id, '*');
    user_id = strtobigint(user_id_buffer);
    
    PGresult* search_player = (PGresult*) { 0 };
    search_player = SQL_query(search_player, "select * from public.player where user_id = %ld", user_id);

    DATABASE_ERROR((PQntuples(search_player) == 0), "This player does not exist!", search_player);
    PQclear(search_player);
  }
  else {
    user_id = event->member->user->id;
  }

  discord_get_user(client, user_id, &ret_user);

  return 0;
}
