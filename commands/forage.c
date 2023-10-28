struct sd_forage_resp {
  struct discord_component buttons[3];
  char custom_ids[3][64];
  char labels[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];

  char description[512];
  char image_url[128];
  char footer_txt[64];
  char footer_url[128];
};

void factor_war(struct sd_player *player, struct sd_scurry *scurry, struct sd_rewards *rewards)
{
  // search for another scurry
  PGresult* opponents = (PGresult*) { 0 };
  opponents = SQL_query(opponents, "select owner_id from public.scurry where owner_id != %ld and war_flag = 1",
      scurry->scurry_owner_id);
  
  if (PQntuples(opponents) > 0)
  {
    int rand_idx = rand() % PQntuples(opponents);
    unsigned long opponent_scurry_id = strtobigint(PQgetvalue(opponents, rand_idx, DB_SCURRY_OWNER_ID));
    
    struct sd_scurry opponent = { 0 };
    load_scurry_struct(&opponent, opponent_scurry_id);

    rewards->stolen_acorns = rewards->acorns;
    opponent.war_acorns -= rewards->stolen_acorns;
    
    player->stolen_acorns += rewards->stolen_acorns;
    scurry->total_stolen_acorns += rewards->stolen_acorns;
  
    if (opponent.war_acorns <= 0) 
    {
      opponent.war_acorns = 0;
      opponent.war_flag = 0;
    }

    update_scurry_row(&opponent);
  }

  PQclear(opponents);
}

void generate_forage_reward(char* sd_description, size_t description_size, struct sd_player *player, struct sd_rewards *rewards)
{
  // case on item type
  switch (rewards->item_type) 
  {
    case TYPE_ACORN_HANDFUL:
      rewards->acorns = genrand(50, 5);
      player->session_data.acorn_handful++;
      break;
    case TYPE_ACORN_MOUTHFUL:
      rewards->acorns = genrand(100, 10);
      player->session_data.acorn_mouthful++;
      break;
    case TYPE_LOST_STASH:
      rewards->acorns = genrand(200, 15);
      rewards->golden_acorns = genrand(50, 5);
      player->session_data.lost_stash++;
      break;
    case TYPE_ACORN_SACK:
      rewards->acorns = genrand(300, 25);
      player->session_data.acorn_sack++;
  }

  if (rewards->acorns) 
  {
    // apply base amount goes to war stash
    if (player->scurry_id > 0)
    {
      struct sd_scurry scurry = { 0 };
      load_scurry_struct(&scurry, player->scurry_id);

      // general chance to either steal or recover war acorns
      int scurry_chance = rand() % MAX_CHANCE;

      if (scurry.war_flag == 0
        && scurry_chance > 65)
      {
        rewards->war_acorns = (scurry.war_acorns + rewards->acorns >= scurry.war_acorn_cap)
            ? scurry.war_acorn_cap - scurry.war_acorns : rewards->acorns;

        scurry.war_acorns += rewards->war_acorns;
      }
      else if (scurry_chance > 65)
        factor_war(player, &scurry, rewards);
    
      update_scurry_row(&scurry);

      rewards->acorns = rewards->acorns * (BASE_ACORN_MULT * (scurry.rank +1) +1);
    }

    // apply base earning to acorn count BEFORE increases
    rewards->acorn_count = rewards->acorns;

    struct sd_buff_status buff_status = { 0 };
    apply_base_rewards(player, rewards, &buff_status);

    // must be right chance for right button
    if (player->buffs.proficiency_acorn == 0 && rand() % MAX_CHANCE > PROFICIENCY_BUFF_CHANCE && player->button_idx == rand() % 3)
    {
      buff_status.received_proficiency_buff = 1;
      player->buffs.proficiency_acorn += genrand(10, 5);
    }

    if (player->buffs.luck_acorn == 0 && rand() % MAX_CHANCE > LUCK_BUFF_CHANCE && player->button_idx == rand() % 3)
    {
      buff_status.received_luck_buff = 1;
      player->buffs.luck_acorn += genrand(5, 5);
    }

    print_rewards(sd_description, description_size, player, rewards, &buff_status);
  }
  else {
    player->session_data.no_acorns++;
    u_snprintf(sd_description, description_size, "\nYou received no earnings! \n");
  }

  struct sd_buff_status buff_status = { 0 }; // for boosted_acorn

  if (player->health < player->max_health
    && rand() % MAX_CHANCE > 70)
  {
    int hp_difference = player->max_health - player->health;
    // scales per strength evolution!
    int health_regen = player->stats.strength_lv +1;

    if (player->squirrel == SKELETAL_SQUIRREL)
    {
      health_regen *= SQUIRREL_CONSTANT;
      // only apply boosted acorn to health regen if squirrel is active
      if (player->buffs.boosted_acorn > 0) {
        health_regen *= BOOSTED_ACORN_CONSTANT;
        player->buffs.boosted_acorn--;
        buff_status.boosted_acorn = true;
      }
    }

    // if difference of player health and max health is less than the health regen ? give the difference : apply regen
    // so players can regen their health completely
    health_regen = (hp_difference < health_regen) ? hp_difference : health_regen;
    player->health += health_regen;

    APPLY_NUM_STR(renew_health, health_regen);
    APPLY_NUM_STR(health, player->health);
    u_snprintf(sd_description, description_size,
      "\n+**%s** "HEALTH" HP (**%s** "HEALTH" HP Left) \n", 
      renew_health, health);
    
    if (player->squirrel == SKELETAL_SQUIRREL && buff_status.boosted_acorn)
      u_snprintf(sd_description, description_size, "\n-**10**%% "BOOSTED_ACORN" Squirrel Charge (**%d**%% left)", player->buffs.boosted_acorn *10);
  }
}

int build_forage_buttons(const struct discord_interaction *event, struct sd_forage_resp *params, struct sd_player *player)
{
  // determine item types first and redirect if necessary
  int button_items[3] = { };
  int chance = 0;

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    chance = rand() % MAX_CHANCE;
    button_items[button_idx] = 
        (chance < JUNK_CHANCE) ? TYPE_NO_ACORNS
      : (chance < COMMON_CHANCE) ? TYPE_ACORN_HANDFUL
      : (chance < UNCOMMON_CHANCE) ? TYPE_ACORN_MOUTHFUL
      : (chance < CONTAINER_CHANCE) ? TYPE_LOST_STASH : TYPE_ACORN_SACK;
  }

  // if all buttons are one reward: introduce random lost stash
  if (button_items[0] == button_items[1] && button_items[1] == button_items[2])
    button_items[rand() % 3] = TYPE_LOST_STASH;

  if (player->squirrel == ANGELIC_SQUIRREL 
    && button_items[player->button_idx] != TYPE_LOST_STASH
    && rand() % MAX_CHANCE > ((player->buffs.boosted_acorn > 0) ? 40 : 70))
  {
    button_items[player->button_idx] = TYPE_LOST_STASH;
  }

  int item_type = ERROR_STATUS;
  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    int current_item = button_items[button_idx];

    // set the emoji
    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                item_types[current_item].emoji_name),
        .id = item_types[current_item].emoji_id
    };

    // apply to button
    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_FORAGE_RESP, button_idx, ERROR_STATUS + 96, event->member->user->id),
      .disabled = true
    };

    // highlight the button selected
    if (player->button_idx == button_idx)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      item_type = current_item;
    }
    else
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
  }

  return item_type;
}

int forage_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event);
  player.button_idx = (event->data->custom_id) ? event->data->custom_id[1] -48 : ERROR_STATUS;

  energy_regen(&player);

  // if player is not on an encounter and encounter chance: generate an encounter
  if (player.encounter == ERROR_STATUS
    && rand() % MAX_CHANCE < ENCOUNTER_CHANCE)
  {
    // select section and encounter!
    player.section = rand() % biomes[player.biome].biome_size;
    player.encounter = rand() % biomes[player.biome].sections[player.section].section_size;

    init_encounter_interaction(event, &player);
    update_player_row(&player);
    return 0;
  }
  // if user tries to skip encounter... keep prompting
  else if (player.encounter != ERROR_STATUS) {
    init_encounter_interaction(event, &player);
    return 0;
  }

  struct sd_forage_resp params = { 0 };

  struct sd_rewards rewards = { 0 };

  rewards.item_type = build_forage_buttons(event, &params, &player);

  struct sd_header_params header = { 0 };

  generate_forage_reward(params.description, sizeof(params.description), &player, &rewards);

  int energy_loss = energy_status(params.description, sizeof(params.description), &player, 2);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "You found %s!", item_types[rewards.item_type].formal_name),
    .description = params.description,
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player.biome].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          item_types[rewards.item_type].file_path)
    },
    .footer = &(struct discord_embed_footer) 
    {
      .text = (energy_loss) ? u_snprintf(params.footer_txt, sizeof(params.footer_txt), "You have %d energy left!", player.energy)
          : u_snprintf(params.footer_txt, sizeof(params.footer_txt), "\n No energy was lost! \n"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ENERGY].file_path)
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = sizeof(params.buttons)/sizeof(*params.buttons)
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

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  player.main_cd = time(NULL) + COOLDOWN;

  update_player_row(&player);

  return 0;
}