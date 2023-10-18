struct sd_encounter_resp {
  struct discord_component buttons[4];
  char custom_ids[4][64];
  char labels[4][64];

  struct discord_emoji emojis[4];
  char emoji_names[4][64];

  char image_url[128];
  char description[1024];
  char footer_txt[64];
  char footer_url[128];
};

void apply_conjured_acorn(struct sd_player *player, struct sd_rewards *rewards)
{
  int give_conjured_acorn = 0;
  switch (player->biome)
  {
    case GRASSLANDS:
      // Match location of Witch Swamp
      if (player->section == 3)
        give_conjured_acorn = 1;
      break;
    case SEEPING_SANDS:
      // Match location of Wormhole
      if (player->section == 4)
        give_conjured_acorn = 1;
      break;
    case NATURE_END:
      // Match location of Breached Woods
      if (player->section == 2)
        give_conjured_acorn = 1;
      break;
    case DEATH_GRIP:
      // Match location of Necrotic Lakes
      if (player->section == 1)
        give_conjured_acorn = 1;
      break;
    case LAST_ACORN:
      // Match location of Death's Locus
      if (player->section == 4)
        give_conjured_acorn = 1;
      break;
  }

  if (give_conjured_acorn)
  {
    rewards->conjured_acorns = (player->button_idx == 2) ? 2 : 1;

    struct tm *info = get_UTC();
    if (info->tm_wday == 5 && info->tm_mday == 13)
      rewards->conjured_acorns *= 2;
    
    if (player->conjured_acorns + rewards->conjured_acorns > 10)
      player->conjured_acorns = 10;
    else
      player->conjured_acorns += rewards->conjured_acorns;

    if (player->conjured_acorns == 10)
      rewards->charge_ready = 1;
  }
}

void complete_encounter(struct sd_encounter_resp *params, struct sd_player *player)
{
  int encounter_idx = 0;
  for (int i = 1; i < player->section +1; i++)
  {
    // count all whole sections
    encounter_idx += biomes[player->biome].sections[player->section - i].section_size;
  }

  int* biome_stories[5] = { 
    &player->story.grasslands, 
    &player->story.seeping_sands, 
    &player->story.nature_end, 
    &player->story.death_grip, 
    &player->story.last_acorn 
  };

  // add progress through current section
  encounter_idx += player->encounter;

  // if encounter idx bit is on or player didn't chance info, this encounter has been seen!
  if ((*biome_stories[player->biome] >> encounter_idx & 0x01) == 1 || rand() % MAX_CHANCE < STORY_CHANCE)
    return;

  // now assume the encounter hasn't been seen!

  // switch encounter idx bit on
  (*biome_stories[player->biome]) |= (1 << encounter_idx);

  u_snprintf(params->description, sizeof(params->description), "\nNew element of the story unlocked! \n"QUEST_MARKER" \"*%s*\" \n", 
      biomes[player->biome].sections[player->section].encounters[player->encounter].context);
}

// void generate_encounter_reward(char* e_description, size_t description_size, struct sd_player *player, struct sd_rewards *rewards)
void generate_encounter_reward(struct sd_encounter_resp *params, struct sd_player *player, struct sd_rewards *rewards)
{
  struct sd_buff_status buff_status = { 0 };

  if (player->button_idx == 2)
  {
    player->golden_acorns -= rewards->encounter_cost;
    player->spent_golden_acorns += rewards->encounter_cost;
  }
  else 
  {
    int health_deduction = (rewards->encounter_cost > player->health) ? player->health : rewards->encounter_cost;
    rewards->is_health = 1;
    player->health -= health_deduction;
    player->session_data.health_loss += health_deduction;
  }

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
      rewards->acorns = genrand(250, 15);
      rewards->golden_acorns = genrand(50, 5);
      player->session_data.lost_stash++;
      break;
    case TYPE_ACORN_SACK:
      rewards->acorns = genrand(300, 25);
      player->session_data.acorn_sack++;
  }

  // only fill when charge isn't ready or in use!
  if (player->buffs.boosted_acorn == 0 && player->conjured_acorns < 10)
    apply_conjured_acorn(player, rewards);

  // apply golden acorn cost boost to acorn count! (3rd button)
  rewards->acorn_count = (player->button_idx == 2) ? rewards->acorns *2 : rewards->acorns;

  apply_base_rewards(player, rewards, &buff_status);

  print_rewards(params->description, sizeof(params->description), player, rewards, &buff_status);

  APPLY_NUM_STR(encounter_cost, rewards->encounter_cost);
  if (rewards->is_health)
  {
    APPLY_NUM_STR(player_health, player->health);

    u_snprintf(params->description, sizeof(params->description),
        "\n-**%s** "BROKEN_HEALTH" HP (**%s** "HEALTH" HP Left) \n", 
        encounter_cost, player_health);
  }

  if (player->health > 0) // player is not dead yet...
  {
    complete_encounter(params, player);
  } 
  else {
    // final acorns are added and then high score is set
    if (player->acorn_count > player->high_acorn_count) {
      u_snprintf(params->description, sizeof(params->description), "\n"ENERGY" **NEW HIGH ACORN COUNT**");
      player->high_acorn_count = player->acorn_count;
    }

    APPLY_NUM_STR(old_acorn_count, player->acorn_count);
    u_snprintf(params->description, sizeof(params->description),
        "\n **%s** "ACORN_COUNT" Final acorn count", old_acorn_count);

    player->health = MAX_HEALTH + generate_factor(player->stats.strength_lv, STRENGTH_FACTOR);

    if (player->acorn_count > (BIOME_INTERVAL * BIOME_SIZE)) {
      player->acorn_count /= 2;
      APPLY_NUM_STR(new_acorn_count, player->acorn_count);
      u_snprintf(params->description, sizeof(params->description),
          "\n"QUEST_MARKER" Your acorn count was set to **%s**! \n", new_acorn_count);

      // Only reset squirrel if new acorn count is less than requirement
      if (player->acorn_count < squirrels[player->squirrel].acorn_count_req)
        player->squirrel = 0;
    }
    else {
      player->acorn_count = 0;
      u_snprintf(params->description, sizeof(params->description),
          "\n"QUEST_MARKER" Your acorn count was reset! \n");
    }
  }

  player->encounter = ERROR_STATUS;
}

void build_encounter_buttons(const struct discord_interaction *event, struct sd_encounter_resp *params, struct sd_player *player, struct sd_rewards *rewards)
{
  struct sd_encounter encounter = biomes[player->biome].sections[player->section].encounters[player->encounter];

  int potential_types[3] = {};

  if (player->squirrel == ANGELIC_SQUIRREL)
  {
    potential_types[0] = (rand() % MAX_CHANCE > 40 -20) ? TYPE_ACORN_MOUTHFUL : TYPE_LOST_STASH; // half damage -- for defense
    potential_types[1] = (rand() % MAX_CHANCE > 20 +20) ? TYPE_LOST_STASH : TYPE_ACORN_SACK; // full damage -- for golden acorns
  }
  else {
    potential_types[0] = (rand() % MAX_CHANCE > 40) ? TYPE_ACORN_MOUTHFUL : TYPE_LOST_STASH; // half damage -- for defense
    potential_types[1] = (rand() % MAX_CHANCE > 20) ? TYPE_LOST_STASH : TYPE_ACORN_SACK; // full damage -- for golden acorns
  }

  potential_types[2] = (rand() % MAX_CHANCE > 20) ? TYPE_ACORN_SACK : TYPE_ACORN_MOUTHFUL; // golden acorns -- for score

  // int potential_types[3] = { 
  //   (rand() % MAX_CHANCE > 40) ? TYPE_ACORN_MOUTHFUL : TYPE_LOST_STASH, // half damage -- for defense
  //   (rand() % MAX_CHANCE > 20) ? TYPE_LOST_STASH : TYPE_ACORN_SACK, // full damage -- for golden acorns
  //   (rand() % MAX_CHANCE > 20) ? TYPE_ACORN_SACK : TYPE_ACORN_MOUTHFUL // golden acorns -- for score
  // };
  
  rewards->item_type = potential_types[player->button_idx];

  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    int current_item = potential_types[button_idx];

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
      // highlight the selected button
      .style = (player->button_idx == button_idx) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
      .emoji = &params->emojis[button_idx],
      // adding player->biome as a character is not a breaking change! It's only encounters that need it
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d_%ld",
                    TYPE_ENCOUNTER_RESP, button_idx, event->member->user->id),
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), encounter.solutions[button_idx]),
      .disabled = true
    };
  }

  params->emojis[3] = (struct discord_emoji) {
    .name = u_snprintf(params->emoji_names[3], sizeof(params->emoji_names[3]), 
            item_types[TYPE_ENCOUNTER].emoji_name),
    .id = item_types[TYPE_ENCOUNTER].emoji_id
  };
  params->buttons[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .emoji = &params->emojis[3],
    .custom_id = u_snprintf(params->custom_ids[3], sizeof(params->custom_ids[3]), "%c%d_%ld*%ld",
      TYPE_BIOME_STORY, player->biome, event->member->user->id, event->member->user->id),
    .label = "Biome Story"
  };
}

void encounter_error(const struct discord_interaction *event, struct sd_player *player)
{
  struct sd_encounter_resp params = { 0 };

  // recall encounter
  int original_biome_idx = event->data->custom_id[4] -48;
  int original_section_idx = event->data->custom_id[3] -48;
  int original_encounter_idx = event->data->custom_id[2] -96;
  struct sd_encounter encounter = biomes[original_biome_idx].sections[original_section_idx].encounters[original_encounter_idx];

  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    params.emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params.emoji_names[button_idx], sizeof(params.emoji_names[button_idx]), 
                item_types[TYPE_ENCOUNTER].emoji_name),
        .id = item_types[TYPE_ENCOUNTER].emoji_id
    };

    params.buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .style = (player->button_idx == button_idx) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
      .emoji = &params.emojis[button_idx],
      .label = u_snprintf(params.labels[button_idx], sizeof(params.labels[button_idx]), encounter.solutions[button_idx]),
      .custom_id = u_snprintf(params.custom_ids[button_idx], sizeof(params.custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_ENCOUNTER_RESP, button_idx, original_encounter_idx + 96, event->member->user->id),
      .disabled = true
    };
  }

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = 3
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = 5
      }
    }
  };
  
  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player->color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },

    .title = u_snprintf(header.title, sizeof(header.title), "%s: %s", 
        biomes[original_biome_idx].sections[original_section_idx].section_name, encounter.name),
    .description = u_snprintf(params.description, sizeof(params.description), 
        "%s \nYou have already responded to this encounter!",
         encounter.conflict),

    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[original_biome_idx].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          item_types[TYPE_NO_ACORNS].file_path)
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
}

int encounter_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event);

  // heal was used!
  if (player.button_idx == 3)
  {
    int full_heal = player.max_health * HEALING_FACTOR;
    int heal_cost = (BUFF_FACTOR * (player.stats.strength_lv + player.stats.strength_lv / STAT_EVOLUTION));

    player.health = (player.health + full_heal > player.max_health) ? player.max_health : player.health + full_heal;
    player.golden_acorns -= heal_cost;
    player.spent_golden_acorns += heal_cost;

    // re-initialize encounter!
    init_encounter_interaction(event, &player);
    update_player_row(&player);
    return 0;
  }
  // error handle if this was a prior encounter and not the current encounter if one is active
  else if (player.encounter == ERROR_STATUS
    || player.encounter != event->data->custom_id[2] - 96)
  {
    // temorarily recall the encounter index through the custom id
    // this value will not be retained by the database!!
    player.encounter = event->data->custom_id[2] - 96;
    encounter_error(event, &player);
    return ERROR_STATUS;
  }

  struct sd_encounter_resp params = { 0 };

  char quantity[64] = { };
  trim_buffer(quantity, sizeof(quantity), event->data->custom_id, '-');
  
  struct sd_rewards rewards = { .encounter_cost = strtoint(quantity) };

  build_encounter_buttons(event, &params, &player, &rewards);

  struct sd_encounter encounter = biomes[player.biome].sections[player.section].encounters[player.encounter];

  // apply encounter description FIRST
  u_snprintf(params.description, sizeof(params.description), encounter.conflict);

  // then add rewards
  generate_encounter_reward(&params, &player, &rewards);
  
  int energy_loss = energy_status(params.description, sizeof(params.description), &player, 2);

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
    .title = u_snprintf(header.title, sizeof(header.title), "%s: %s", 
        biomes[player.biome].sections[player.section].section_name, encounter.name),

    .description = params.description,
    
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player.biome].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          encounter.file_path)
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

  update_player_row(&player);

  return 0;
}