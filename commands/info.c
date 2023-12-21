struct sd_squirrel_info 
{
  struct discord_component buttons[2];
  char custom_ids[2][64];

  struct discord_emoji emojis[2];
  char emoji_names[2][64];

  struct discord_embed_field fields[4];
  char field_names[4][64];
  char field_values[4][1024];
};

void build_general_info(struct sd_squirrel_info *params, struct sd_player *player, struct sd_scurry *scurry)
{
  APPLY_NUM_STR(acorn_count, player->acorn_count);
  APPLY_NUM_STR(req_acorn_count, BIOME_INTERVAL * (player->biome_num +1) );
  APPLY_NUM_STR(high_acorn_count, player->high_acorn_count);

  struct sd_file_data biome_icon = biomes[player->biome].biome_icon;
  struct sd_file_data squirrel = squirrels[player->squirrel].squirrel;

  params->fields[0].name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "General Info");

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char* seasons[4] = {"Spring", "Summer", "Fall", "Winter"};

  struct tm *info = get_UTC();
  char* season = seasons[(info->tm_mday/7 < 4) ? info->tm_mday/7 : 3];
  char* month = months[(info->tm_mday < 21) ? info->tm_mon : (info->tm_mon +1 == 12) ? 0 : info->tm_mon +1];
  int end_day = (info->tm_mday < 7) ? 7 : (info->tm_mday < 14) ? 14 : (info->tm_mday < 21) ? 21 : 1;

  // end_day can only be 1, otherwise it's greater than 3
  char* suffix = (end_day == 1) ? "st" : "th";

  u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
      " "INDENT" "ACORN_COUNT" Acorn Count: **%s**/%s \n"
      " "INDENT" <:%s:%ld> Biome: **%s** \n"
      " "INDENT" <:%s:%ld> Squirrel: **%s** \n"
      " "INDENT" "LEADER" High Score: **%s** \n"
      " "INDENT" "WEATHER" Season: **%s** (Ends on %s **%d%s**) \n",
      acorn_count, req_acorn_count,
      biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name,
      squirrel.emoji_name, squirrel.emoji_id, squirrel.formal_name,
      (player->high_acorn_count > 0) ? high_acorn_count : acorn_count,
      season, month, end_day, suffix);

  if (player->scurry_id > 0)
    u_snprintf(params->field_values[0], sizeof(params->field_values[0]),
        " "INDENT" "GUILD_ICON" Current Guild: **%s** \n", scurry->scurry_name);

  params->fields[0].value = params->field_values[0];
}

void build_resources_info(struct sd_squirrel_info *params, struct sd_player *player)
{
  APPLY_NUM_STR(acorns, player->acorns);
  APPLY_NUM_STR(golden_acorns, player->golden_acorns);
  APPLY_NUM_STR(health, player->health);
  APPLY_NUM_STR(max_health, player->max_health);

  params->fields[1].name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Resources");

  u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
      " "INDENT" "ENERGY" Energy: **%d**/%d \n"
      " "INDENT" "HEALTH" Health: **%s**/%s \n"
      " "INDENT" "ACORNS" Acorns: **%s** \n",
      player->energy, MAX_ENERGY, health, max_health, acorns);

  if (player->stored_golden_acorns > 0)
  {
    APPLY_NUM_STR(stored_golden_acorns, player->stored_golden_acorns);

    u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
        " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** ("LOST_STASH" **%s**) \n",
        golden_acorns, stored_golden_acorns);
  }
  else {
    u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
        " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n",
        golden_acorns);
  }

  // conditional info
  struct tm *info = get_UTC();
  if (info->tm_mday > 21) {
    APPLY_NUM_STR(catnip, player->catnip);
    u_snprintf(params->field_values[1], sizeof(params->field_values[1]),
        " "INDENT" "CATNIP" Catnip: **%s** \n", catnip);
  }

  params->fields[1].value = params->field_values[1];
}

void build_stats_info(struct sd_squirrel_info *params, struct sd_player *player)
{
  params->fields[2].name = u_snprintf(params->field_names[2], sizeof(params->field_names[2]), "Squirrel Stats");

  // proficiency stat (init field)
  float proficiency_value = generate_factor(STAT_PROFICIENCY, player->stats.proficiency_lv);
  u_snprintf(params->field_values[2], sizeof(params->field_values[2]), 
      " "INDENT" "PROFICIENCY_ICON" *%s*  (Lv **%d**)  x**%0.1f** \n",
      stats[STAT_PROFICIENCY].stat.formal_name, player->stats.proficiency_lv, proficiency_value);

  // strength stat
  float strength_value = generate_factor(STAT_STRENGTH, player->stats.strength_lv);
  u_snprintf(params->field_values[2], sizeof(params->field_values[2]),
      " "INDENT" "STRENGTH_ICON" *%s*  (Lv. **%d**)  +**%0.0f** \n",
      stats[STAT_STRENGTH].stat.formal_name, player->stats.strength_lv, strength_value);

  // luck stat
  float luck_value = generate_factor(STAT_LUCK, player->stats.luck_lv);
  u_snprintf(params->field_values[2], sizeof(params->field_values[2]),
      " "INDENT" "LUCK_ICON" *%s*  (Lv **%d**)  x**%0.1f** \n",
      stats[STAT_LUCK].stat.formal_name, player->stats.luck_lv, luck_value);

  // set the latest buffer to the pointer
  params->fields[2].value = params->field_values[2];
}

void build_buffs_info(struct sd_squirrel_info *params, struct sd_player *player)
{
  if (player->buffs.proficiency_acorn == 0
    && player->buffs.luck_acorn == 0)
  {
    params->fields[3] = (struct discord_embed_field) {
      .name = u_snprintf(params->field_names[3], sizeof(params->field_names[3]), "Squirrel Buffs"),
      .value = u_snprintf(params->field_values[3], sizeof(params->field_values[3]), 
          " "OFF_ARROW" *No enchanted acorns are currrently active*")
    };
    return;
  }

  int buff_durations[2] = {
    player->buffs.proficiency_acorn,
    player->buffs.luck_acorn,
  };
  
  int active_buffs[2] = {
    BUFF_PROFICIENCY_ACORN,
    BUFF_LUCK_ACORN,
  };

  params->fields[3] = (struct discord_embed_field) { 
    .name = u_snprintf(params->field_names[3], sizeof(params->field_names[3]), "Squirrel Buffs") 
  };

  for (int buff_idx = 0; buff_idx < 2; buff_idx++)
  {
    struct sd_file_data active_buff = enchanted_acorns[active_buffs[buff_idx]];
    if (buff_durations[buff_idx] > 0)
    {
      APPLY_NUM_STR(duration, buff_durations[buff_idx]);
      u_snprintf(params->field_values[3], sizeof(params->field_values[3]),
          " "INDENT" <:%s:%ld> *%s* (**%s**) \n",
          active_buff.emoji_name, active_buff.emoji_id, active_buff.formal_name, duration);
    }
  }
  // set the latest buffer to the pointer
  params->fields[3].value = params->field_values[3];
}

void p_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  struct sd_player player = { 0 };
  load_player_struct(&player, user->id, event->data->custom_id);
  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_header_params header = { 0 };
  struct sd_squirrel_info params = { 0 };

  energy_regen(&player);

  build_general_info(&params, &player, &scurry);
  build_resources_info(&params, &player);
  build_stats_info(&params, &player);
  build_buffs_info(&params, &player);

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
          (player.designer_squirrel == ERROR_STATUS) 
            ? squirrels[player.squirrel].squirrel.file_path 
            : designer_squirrels[player.designer_squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 4
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

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);
}

// only sent when NOT requesting another user
int info_from_buttons(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_header_params header = { 0 };
  struct sd_squirrel_info params = { 0 };

  energy_regen(&player);

  build_general_info(&params, &player, &scurry);
  build_resources_info(&params, &player);
  build_stats_info(&params, &player);
  build_buffs_info(&params, &player);

  if (player.button_idx == 5)
  {
    player.buffs.boosted_acorn += squirrels[player.squirrel].boosted_duration;
    player.conjured_acorns -= SQUIRREL_BOOST_COST;
  }

  struct sd_file_data squirrel_dir = (player.designer_squirrel == ERROR_STATUS) 
      ? squirrels[player.squirrel].squirrel
      : designer_squirrels[player.designer_squirrel].squirrel;

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
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, squirrel_dir.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 4
    }
  };

  struct sd_secondary secondary = { 0 };

  generate_secondary_buttons(event, &secondary, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  params.emojis[0] = (struct discord_emoji)
  {
    .name = u_snprintf(params.emoji_names[0], sizeof(params.emoji_names[0]), 
        items[ITEM_ACORN_COUNT].emoji_name),
    .id = items[ITEM_ACORN_COUNT].emoji_id
  };
  params.buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .custom_id = u_snprintf(params.custom_ids[0], sizeof(params.custom_ids[0]), "%c0.%ld",
        TYPE_SESSION_INFO, event->member->user->id),
    .label = "Statistics",
    .emoji = &params.emojis[0]
  };

  params.emojis[1] = (struct discord_emoji)
  {
    .name = u_snprintf(params.emoji_names[1], sizeof(params.emoji_names[1]), 
        item_types[TYPE_NO_ACORNS].emoji_name),
    .id = item_types[TYPE_NO_ACORNS].emoji_id
  };
  params.buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .custom_id = u_snprintf(params.custom_ids[1], sizeof(params.custom_ids[1]), "%c0.%ld",
        TYPE_PLAYER_HELP, event->member->user->id),
    .label = "Help",
    .emoji = &params.emojis[1]
  };

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = secondary.buttons,
        .size = secondary.button_size
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = 2
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
          .size = 3
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
          .size = 3
        }
      }

    };

    char values[16384];
    discord_interaction_response_to_json(values, sizeof(values), &interaction);
    fprintf(stderr, "%s \n", values);

    discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

    update_player_row(&player);
  }

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

    ERROR_DATABASE_RET((PQntuples(search_player) == 0), "This player does not exist!", search_player);
    PQclear(search_player);
  }
  else if (event->data->custom_id)
  {
    char user_id_buffer[64] = { };
    trim_buffer(user_id_buffer, sizeof(user_id_buffer), event->data->custom_id, '*');
    user_id = strtobigint(user_id_buffer);
    
    PGresult* search_player = (PGresult*) { 0 };
    search_player = SQL_query(search_player, "select * from public.player where user_id = %ld", user_id);

    ERROR_DATABASE_RET((PQntuples(search_player) == 0), "This player does not exist!", search_player);
    PQclear(search_player);
  }
  else {
    user_id = event->member->user->id;
  }

  discord_get_user(client, user_id, &ret_user);

  return 0;
}
