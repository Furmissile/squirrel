struct sd_forage_resp {
  struct discord_component buttons[4];
  char custom_ids[4][64];

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
      rewards->acorns = genrand(25, 15);
      break;
    case TYPE_ACORN_MOUTHFUL:
      rewards->acorns = genrand(75, 25);
      break;
    case TYPE_LOST_STASH:
      rewards->acorns = genrand(125, 25);
      rewards->golden_acorns = genrand(50, 25);
      break;
    case TYPE_ACORN_SACK:
      rewards->acorns = genrand(150, 50);
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

    // if (blood_mode is on...)

    struct sd_buff_status buff_status = { 0 };
    apply_base_rewards(player, rewards, &buff_status);
  
    print_rewards(sd_description, description_size, player, rewards, &buff_status);
  }
  else 
    u_snprintf(sd_description, description_size, "\nYou received no earnings! \n");

  struct sd_buff_status buff_status = { 0 }; // for boosted_acorn

  if (player->health < player->max_health
    && rand() % MAX_CHANCE > 80)
  {
    int hp_difference = player->max_health - player->health;
    // scales per strength evolution!
    int health_regen = BASE_HEALTH_REGEN * (player->stats.strength_lv/STAT_EVOLUTION +1);

    if (player->squirrel == SKELETAL_SQUIRREL)
    {
      health_regen *= 2;
      // only apply boosted acorn to health regen if squirrel is active
      if (player->buffs.boosted_acorn > 0) {
        health_regen *= 1.5;
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
      "\n+**%s** "HEALTH" HP (**%s** "HEALTH" HP Left) \n%s", 
      renew_health, health,
      (buff_status.boosted_acorn) ? "-**1** "BOOSTED_ACORN" Boosted Acorn \n" : " " );
  }
}

int build_forage_buttons(const struct discord_interaction *event, struct sd_forage_resp *params)
{
  // determine item types first and redirect if necessary
  int button_items[3] = { };
  int failure = 0;
  int chance = 0;

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    chance = rand() % MAX_CHANCE;

    if (chance < JUNK_CHANCE) {
      button_items[button_idx] = TYPE_NO_ACORNS;
      failure++;
    }
    else
      button_items[button_idx] = 
        (chance < COMMON_CHANCE) ? TYPE_ACORN_HANDFUL
        : (chance < UNCOMMON_CHANCE) ? TYPE_ACORN_MOUTHFUL
        : (chance < CONTAINER_CHANCE) ? TYPE_LOST_STASH : TYPE_ACORN_SACK;
  }

  chance = rand() % MAX_CHANCE;
  if (failure == 3) 
      button_items[rand() % 3] = 
        (chance < COMMON_CHANCE) ? TYPE_ACORN_HANDFUL
        : (chance < UNCOMMON_CHANCE) ? TYPE_ACORN_MOUTHFUL
        : (chance < CONTAINER_CHANCE) ? TYPE_LOST_STASH : TYPE_ACORN_SACK;

  int item_type = ERROR_STATUS;
  for (int button_idx = 1; button_idx < 4; button_idx++) 
  {
    int current_item = button_items[button_idx -1];

    // set the emoji
    params->emojis[button_idx -1] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx -1], sizeof(params->emoji_names[button_idx -1]), 
                item_types[current_item].emoji_name),
        .id = item_types[current_item].emoji_id
    };

    // apply to button
    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx -1],
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_FORAGE_RESP, button_idx -1, ERROR_STATUS + 96, event->member->user->id),
      .disabled = true
    };

    // highlight the button selected
    if (event->data->custom_id[1] -48 == button_idx -1)
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
  load_player_struct(&player, event->member->user->id); 

  energy_regen(&player);

  int encounter_chance = (player.vengeance_flag) ? 50 : ENCOUNTER_CHANCE;

  if (player.encounter == ERROR_STATUS
    && rand() % MAX_CHANCE < encounter_chance)
  {
    player.encounter = rand() % biomes[player.biome].encounter_size;
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
  
  params.buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Forage again!",
    .custom_id = u_snprintf(params.custom_ids[0], sizeof(params.custom_ids[0]), "%c3%c_%ld",
        TYPE_FORAGE_INIT, ERROR_STATUS + 96, event->member->user->id),
  };

  struct sd_rewards rewards = { 0 };

  rewards.item_type = build_forage_buttons(event, &params);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 4
    }
  };

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
    .footer = (energy_loss) ?
      &(struct discord_embed_footer) {
        .text = u_snprintf(params.footer_txt, sizeof(params.footer_txt), "You have %d energy left!", player.energy),
        .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ENERGY].file_path)
      }
      : &(struct discord_embed_footer) { 0 }
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
        .array = &action_rows,
        .size = 1
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