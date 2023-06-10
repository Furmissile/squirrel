struct sd_encounter_resp {
  struct discord_component buttons[4];
  char custom_ids[4][64];
  char labels[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];

  char image_url[128];
  char description[512];
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
      if (player->encounter > 9 && player->encounter < 13)
        give_conjured_acorn = 1;
      break;
    case SEEPING_SANDS:
      // Match location of Wormhole
      if (player->encounter > 10 && player->encounter < 13)
        give_conjured_acorn = 1;
      break;
    case NATURE_END:
      // Match location of Breached Woods
      if (player->encounter > 5 && player->encounter < 8)
        give_conjured_acorn = 1;
      break;
    case DEATH_GRIP:
      // Match location of Necrotic Lakes
      if (player->encounter > 3 && player->encounter < 6)
        give_conjured_acorn = 1;
      break;
    case LAST_ACORN:
      // Match location of Death's Locus
      if (player->encounter > 11 && player->encounter < 15)
        give_conjured_acorn = 1;
      break;
  }

  if (give_conjured_acorn)
  {
    rewards->conjured_acorns = (rewards->item_type == TYPE_ACORN_MOUTHFUL) ? 1 : 2;
    player->conjured_acorns += rewards->conjured_acorns;
  }
}

void generate_encounter_reward(char* e_description, size_t description_size, struct sd_player *player, struct sd_rewards *rewards)
{
  int health_loss = 0;

  // case on item type
  switch (rewards->item_type) 
  {
    case TYPE_ACORN_MOUTHFUL:
      rewards->acorns = genrand(75, 25);
      rewards->golden_acorns = genrand(50, 25);
      break;
    case TYPE_HEALTH_LOSS:
      health_loss = genrand(10, 5) + (player->biome_num * BIOME_DAMAGE);
      break;
    case TYPE_ACORN_SACK:
      rewards->acorns = genrand(150, 50);
      rewards->golden_acorns = genrand(100, 50);
  }

  if (rewards->acorns) {
    apply_conjured_acorn(player, rewards);

    struct sd_buff_status buff_status = { 0 };
    apply_base_rewards(player, rewards, &buff_status);
    print_rewards(e_description, description_size, player, rewards, &buff_status);
  }
  else {
    u_snprintf(e_description, description_size, "\nYou received no earnings! \n");

    health_loss = (health_loss > player->health) ? player->health : health_loss;
    player->health -= health_loss;

    APPLY_NUM_STR(reward, health_loss);
    APPLY_NUM_STR(player_health, player->health);

    u_snprintf(e_description, description_size,
        "\n-**%s** "BROKEN_HEALTH" HP (**%s** "HEALTH" HP Left) \n", 
        reward, player_health );

    if (player->health <= 0
      || player->vengeance_flag) 
    {
      // final acorns are added and then high score is set
      if (player->acorn_count > player->high_acorn_count) {
        u_snprintf(e_description, description_size, "\n"ENERGY" **NEW HIGH ACORN COUNT**");
        player->high_acorn_count = player->acorn_count;
      }

      APPLY_NUM_STR(old_acorn_count, player->acorn_count);
      u_snprintf(e_description, description_size,
          "\n **%s** "ACORN_COUNT" Final acorn count", old_acorn_count);

      player->health = MAX_HEALTH + generate_factor(player->stats.strength_lv, STRENGTH_FACTOR);

      if (player->acorn_count > (BIOME_INTERVAL * BIOME_SIZE)) {
        player->acorn_count /= 2;
        APPLY_NUM_STR(new_acorn_count, player->acorn_count);
        u_snprintf(e_description, description_size,
            "\n"QUEST_MARKER" Your acorn count was set to **%s**! \n", new_acorn_count);

        // Only reset squirrel if new acorn count is less than requirement
        if (player->acorn_count < squirrels[player->squirrel].acorn_count_req)
          player->squirrel = 0;
      }
      else {
        player->acorn_count = 0;
        u_snprintf(e_description, description_size,
            "\n"QUEST_MARKER" Your acorn count was reset! \n");
      }

      player->vengeance_flag = 0;
    }

  }
  
  player->encounter = ERROR_STATUS;
}

int build_encounter_buttons(const struct discord_interaction *event, struct sd_encounter_resp *params, struct sd_player *player)
{
  // determine item types first and redirect if necessary
  int button_items[3] = { };
  int failure = 0;
  int chance = 0;

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    chance = rand() % MAX_CHANCE;

    if (chance < HEALTH_LOSS_CHANCE) {
      button_items[button_idx] = TYPE_HEALTH_LOSS;
      failure++;
    }
    else
      button_items[button_idx] = 
        (chance < NORMAL_CHANCE) ? TYPE_ACORN_MOUTHFUL : TYPE_ACORN_SACK;
  }

  chance = rand() % MAX_CHANCE;
  if (failure == 3) 
      button_items[rand() % 3] = 
        (chance < NORMAL_CHANCE) ? TYPE_ACORN_MOUTHFUL : TYPE_ACORN_SACK;

  struct sd_encounter encounter = biomes[player->biome].encounters[player->encounter];

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
      // adding player->biome as a character is not a breaking change! It's only encounters that need it
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c%d_%ld",
                    TYPE_ENCOUNTER_RESP, button_idx -1, player->encounter + 96, player->biome, event->member->user->id),
      .label = u_snprintf(params->labels[button_idx -1], sizeof(params->labels[button_idx -1]), encounter.solutions[button_idx -1]),
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

void encounter_error(const struct discord_interaction *event, struct sd_player *player)
{
  struct sd_encounter_resp params = { 0 };

  params.buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Forage again!",
    .custom_id = u_snprintf(params.custom_ids[0], sizeof(params.custom_ids[0]), "%c3%c_%ld",
        TYPE_FORAGE_INIT, ERROR_STATUS + 96, event->member->user->id),
  };

  // recall encounter
  int original_biome_idx = event->data->custom_id[3] -48;
  int original_encounter_idx = event->data->custom_id[1] -48;
  struct sd_encounter encounter = biomes[original_biome_idx].encounters[original_encounter_idx];

  for (int button_idx = 1; button_idx < 4; button_idx++) 
  {
    params.emojis[button_idx -1] = (struct discord_emoji) {
        .name = u_snprintf(params.emoji_names[button_idx -1], sizeof(params.emoji_names[button_idx -1]), 
                item_types[TYPE_ENCOUNTER].emoji_name),
        .id = item_types[TYPE_ENCOUNTER].emoji_id
    };

    params.buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_SECONDARY,
      .emoji = &params.emojis[button_idx - 1],
      .label = u_snprintf(params.labels[button_idx -1], sizeof(params.labels[button_idx -1]), encounter.solutions[button_idx -1]),
      .custom_id = u_snprintf(params.custom_ids[button_idx], sizeof(params.custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_ENCOUNTER_RESP, button_idx -1, original_encounter_idx + 96, event->member->user->id),
      .disabled = true
    };
  }

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 4
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

    .title = u_snprintf(header.title, sizeof(header.title), encounter.name),
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
        .array = &action_rows,
        .size = 1
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
  load_player_struct(&player, event->member->user->id); 

  // error handle if this was a prior encounter and not the current encounter if one is active
  if (player.encounter == ERROR_STATUS
    || player.encounter != event->data->custom_id[2] - 96)
  {
    // temorarily recall the encounter index through the custom id
    // this value will not be retained by the database!!
    player.encounter = event->data->custom_id[2] - 96;
    encounter_error(event, &player);
    return ERROR_STATUS;
  }

  struct sd_encounter_resp params = { 0 };

  params.buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Forage again!",
    .custom_id = u_snprintf(params.custom_ids[0], sizeof(params.custom_ids[0]), "%c3%c_%ld",
        TYPE_FORAGE_INIT, ERROR_STATUS + 96, event->member->user->id),
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 4
    }
  };

  struct sd_rewards rewards = { 0 };

  rewards.item_type = build_encounter_buttons(event, &params, &player);

  struct sd_encounter encounter = biomes[player.biome].encounters[player.encounter];

  // apply encounter description FIRST
  u_snprintf(params.description, sizeof(params.description), encounter.conflict);

  generate_encounter_reward(params.description, sizeof(params.description), &player, &rewards);
  
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
    .title = u_snprintf(header.title, sizeof(header.title), encounter.name),
    .description = params.description,

    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player.biome].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          encounter.file_path)
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

  update_player_row(&player);

  return 0;
}