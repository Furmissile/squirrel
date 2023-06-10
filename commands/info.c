struct sd_squirrel_info 
{
  struct discord_embed_field fields[2];
  char field_names[2][64];
  char field_values[2][512];

  char description[1024];

  char footer_text[64];
  char footer_url[128];
};

void build_general_info(struct sd_squirrel_info *params, struct sd_player *player, struct sd_scurry *scurry)
{
  APPLY_NUM_STR(health, player->health);
  APPLY_NUM_STR(max_health, player->max_health);
  APPLY_NUM_STR(acorns, player->acorns);
  APPLY_NUM_STR(golden_acorns, player->golden_acorns);
  APPLY_NUM_STR(acorn_count, player->acorn_count);
  APPLY_NUM_STR(req_acorn_count, BIOME_INTERVAL * (player->biome_num +1) );
  APPLY_NUM_STR(high_acorn_count, player->high_acorn_count);
  APPLY_NUM_STR(conjured_acorns, player->conjured_acorns);

  struct sd_file_data biome_icon = biomes[player->biome].biome_icon;

  char is_vengeance[256] = { };

  if (player->vengeance_flag)
    u_snprintf(is_vengeance, sizeof(is_vengeance), 
        " "INDENT" "HEALTH" Health: **1**/1 \n"
        " "INDENT" "BROKEN_HEALTH" Vengeance Mode: "QUEST_MARKER" **Active** \n");
  else
    u_snprintf(is_vengeance, sizeof(is_vengeance),
        " "INDENT" "HEALTH" Health: **%s**/%s \n"
        " "INDENT" "BROKEN_HEALTH" Vengeance Mode: "HELP_MARKER" **Inactive** \n",
        health, max_health);

  u_snprintf(params->description, sizeof(params->description), 
      "**General Info** \n"
      " "INDENT" "ENERGY" Energy: **%d**/%d \n"
      "%s"
      " "INDENT" "ACORNS" Acorns: **%s** \n"
      " "INDENT" "ACORN_COUNT" Acorn Count: **%s**/%s \n"
      " "INDENT" <:%s:%ld> Biome: **%s** \n"
      " "INDENT" "LEADER" High Score: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
      " "INDENT" "CONJURED_ACORNS" Conjured Acorns: **%s** \n" ,
      player->energy, MAX_ENERGY, 
      is_vengeance, acorns, 
      acorn_count, req_acorn_count,
      biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name,
      (player->high_acorn_count > 0) ? high_acorn_count : acorn_count,
      golden_acorns, conjured_acorns);

  // conditional info
  struct tm *info = get_UTC();
  if (info->tm_mday > 21) {
    APPLY_NUM_STR(catnip, player->catnip);
    u_snprintf(params->description, sizeof(params->description),
        " "INDENT" "CATNIP" Catnip: **%s** \n", catnip);
  }

  if (player->scurry_id > 0)
    u_snprintf(params->description, sizeof(params->description),
        " "INDENT" "GUILD_ICON" Current Guild: **%s** \n", scurry->scurry_name);
}

void build_stats_info(struct sd_squirrel_info *params, struct sd_player *player)
{
  params->fields[0].name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Squirrel Stats");

  // proficiency stat (init field)
  float proficiency_value = generate_factor(player->stats.proficiency_lv, PROFICIENCY_FACTOR);
  u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
      " "INDENT" <:%s:%ld> *%s*  (Lv **%d**)  x**%0.1f** \n",
      stats[STAT_PROFICIENCY].stat.emoji_name, stats[STAT_PROFICIENCY].stat.emoji_id,
      stats[STAT_PROFICIENCY].stat.formal_name, player->stats.proficiency_lv, proficiency_value);

  // luck stat
  float luck_value = generate_factor(player->stats.luck_lv, LUCK_FACTOR);
  u_snprintf(params->field_values[0], sizeof(params->field_values[0]),
      " "INDENT" <:%s:%ld> *%s*  (Lv **%d**)  x**%0.1f** \n",
      stats[STAT_LUCK].stat.emoji_name, stats[STAT_LUCK].stat.emoji_id,
      stats[STAT_LUCK].stat.formal_name, player->stats.luck_lv, luck_value);

  // strength stat
  float strength_value = generate_factor(player->stats.strength_lv, STRENGTH_FACTOR);
  u_snprintf(params->field_values[0], sizeof(params->field_values[0]),
      " "INDENT" <:%s:%ld> *%s*  (Lv. **%d**)  +**%0.0f** \n",
      stats[STAT_STRENGTH].stat.emoji_name, stats[STAT_STRENGTH].stat.emoji_id,
      stats[STAT_STRENGTH].stat.formal_name, player->stats.strength_lv, strength_value);

  // set the latest buffer to the pointer
  params->fields[0].value = params->field_values[0];
}

void build_buffs_info(struct sd_squirrel_info *params, struct sd_player *player)
{

  if (player->buffs.proficiency_acorn == 0
    && player->buffs.luck_acorn == 0
    && player->buffs.boosted_acorn == 0)
  {
    params->fields[1] = (struct discord_embed_field) {
      .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Squirrel Buffs"),
      .value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
          " "OFF_ARROW" *No enchanted acorns are currrently active*")
    };
    return;
  }

  int buff_durations[3] = {
    player->buffs.proficiency_acorn,
    player->buffs.luck_acorn,
    player->buffs.boosted_acorn
  };
  
  int active_buffs[3] = {
    BUFF_PROFICIENCY_ACORN,
    BUFF_LUCK_ACORN,
    BUFF_BOOSTED_ACORN
  };

  params->fields[1] = (struct discord_embed_field) { 
    .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Squirrel Buffs") 
  };

  for (int buff_idx = 0; buff_idx < 3; buff_idx++)
  {
    struct sd_file_data active_buff = enchanted_acorns[active_buffs[buff_idx]];
    if (buff_durations[buff_idx] > 0)
    {
      APPLY_NUM_STR(duration, buff_durations[buff_idx]);
      u_snprintf(params->field_values[1], sizeof(params->field_values[1]),
          "<:%s:%ld> *%s* (**%s**) \n",
          active_buff.emoji_name, active_buff.emoji_id, active_buff.formal_name, duration);
    }
  }
  // set the latest buffer to the pointer
  params->fields[1].value = params->field_values[1];
}

void p_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  struct sd_player player = { 0 };
  load_player_struct(&player, user->id);
  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_header_params header = { 0 };
  struct sd_squirrel_info params = { 0 };

  energy_regen(&player);

  build_general_info(&params, &player, &scurry);
  build_stats_info(&params, &player);
  build_buffs_info(&params, &player);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          user->id, user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Player Info"),
    .description = params.description,
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 2
    },
    .footer = &(struct discord_embed_footer)
    {
      .text = params.footer_text,
      .icon_url = params.footer_url
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
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);
}

int info_interaction(const struct discord_interaction *event)
{
  struct discord_ret_user ret_user = {
    .done = &p_info,
    .fail = &not_user,
    .keep = event
  };

  unsigned long user_id;
  if (event->data->options) 
  {
    char user_id_buffer[64] = { };
    trim_user_id(user_id_buffer, sizeof(user_id_buffer), event->data->options->array[0].value);
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
