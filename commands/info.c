void create_info_interaction(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

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

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg);
}

enum INFO_FORMAT {
  INFO_GENERAL,
  INFO_STATS,
  INFO_BUFFS,
  INFO_SIZE
};

void p_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;
  struct sd_message *discord_msg = resp->data;

  struct discord_embed *embed = discord_msg->embed;

  player = load_player_struct(user->id);
  scurry = load_scurry_struct(player.scurry_id);
  energy_regen();

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        user->id, user->avatar) );

  embed->color = player.color;

  embed->title = format_str(SIZEOF_TITLE, "Player Info");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = INFO_SIZE;
  embed->fields->array = calloc(INFO_SIZE, sizeof(struct discord_embed_field));

  struct sd_file_data biome_icon = biomes[player.biome].biome_icon;

  embed->fields->array[INFO_GENERAL].name = format_str(SIZEOF_TITLE, "General Stats");
  embed->fields->array[INFO_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "INDENT" "ENERGY" Energy: **%d**/%d \n"
      " "INDENT" "HEALTH" Health: **%s**/%s \n"
      " "INDENT" "ACORNS" Acorns: **%s** \n"
      " "INDENT" "ACORN_COUNT" Acorn Count: **%s**/%s \n"
      " "INDENT" <:%s:%ld> Biome: **%s** \n"
      " "INDENT" "LEADER" High Score: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n",
      player.energy, MAX_ENERGY, 
      num_str(player.health), num_str(player.max_health),
      num_str(player.acorns), num_str(player.acorn_count), num_str(BIOME_INTERVAL * (player.biome_num +1) ),
      biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name,
      (player.high_acorn_count > 0) ? num_str(player.high_acorn_count) : num_str(player.acorn_count),
      num_str(player.golden_acorns) );

  struct tm *info = get_UTC();
  if (info->tm_mday > 21)
    ADD_TO_BUFFER(embed->fields->array[INFO_GENERAL].value, SIZEOF_FIELD_VALUE,
        " "INDENT" "CATNIP" Catnip: **%s** \n", num_str(player.events.catnip) );

  if (player.scurry_id > 0)
    ADD_TO_BUFFER(embed->fields->array[INFO_GENERAL].value, SIZEOF_FIELD_VALUE,
        " "INDENT" "GUILD_ICON" Current Guild: **%s** \n", scurry.scurry_name);

  /* Loads squirrel stats */
  char player_stat_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < STAT_SIZE; i++)
  {
    char* stahr_type = (*stats[i].stat_ptr < BRONZE_BRACKET) ? BRONZE_STAHR
      : (*stats[i].stat_ptr < SILVER_BRACKET) ? SILVER_STAHR : STAHR;

    float stat_value = generate_factor(*(stats[i].stat_ptr), stats[i].value_mult);

    ADD_TO_BUFFER(player_stat_field, SIZEOF_FIELD_VALUE,
        (i == STAT_STRENGTH) ? " "INDENT" <:%s:%ld> *%s* (Lv **%d** %s) +**%0.0f** \n" 
            : " "INDENT" <:%s:%ld> *%s* (Lv **%d** %s) x**%0.1f** \n",
        stats[i].stat.emoji_name, stats[i].stat.emoji_id,
        stats[i].stat.formal_name, *stats[i].stat_ptr, stahr_type, stat_value );
  }

  embed->fields->array[INFO_STATS].name = format_str(SIZEOF_TITLE, "Squirrel Stats");
  embed->fields->array[INFO_STATS].value = format_str(SIZEOF_FIELD_VALUE, player_stat_field);

  char player_buffs_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < BUFFS_SIZE; i++)
    if (*enchanted_acorns[i].stat_ptr > 0)
      ADD_TO_BUFFER(player_buffs_field, SIZEOF_FIELD_VALUE,
          "<:%s:%ld> *%s* (**%s**) \n",
          enchanted_acorns[i].item.emoji_name, enchanted_acorns[i].item.emoji_id, enchanted_acorns[i].item.formal_name,
          num_str(*enchanted_acorns[i].stat_ptr) );
  
  if (player.buffs.proficiency_acorn == 0
    && player.buffs.luck_acorn == 0
    && player.buffs.defense_acorn == 0)
    ADD_TO_BUFFER(player_buffs_field, SIZEOF_FIELD_VALUE,
        ""OFF_ARROW" *No enchanted acorns are currrently active*");

  embed->fields->array[INFO_BUFFS].name = format_str(SIZEOF_TITLE, "Squirrel Buffs");
  embed->fields->array[INFO_BUFFS].value = format_str(SIZEOF_FIELD_VALUE, player_buffs_field);

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, squirrels[player.squirrel].squirrel.file_path);

  create_info_interaction(event, discord_msg);
  update_player_row(player);
}

void info_error(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  // no need to call create_info_interaction since error_messsage already responded!
  error_message(event, "This is an invalid player!");
}


/* Listens for slash command interactions */
int info_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  struct discord_ret_user ret_user = {
    .done = &p_info,
    .fail = &info_error,
    .data = discord_msg,
    .keep = event
  };

  unsigned long user_id;
  if (event->data->options) {
    user_id = strtobigint(trim_user_id(event->data->options->array[0].value));

    PGresult* search_player = SQL_query(DB_ACTION_SEARCH, "select * from public.player where user_id = %ld", user_id);

    ERROR_DATABASE_RET((PQntuples(search_player) == 0), "This player does not exist!", search_player);
    PQclear(search_player);
  }
  else {
    user_id = event->member->user->id;
  }

  discord_get_user(client, user_id, &ret_user);

  return 0;
}