struct sd_bunny_shop {
  struct discord_component buttons[BUNNY_STORE_SIZE];
  char custom_ids[BUNNY_STORE_SIZE][64];
  char labels[BUNNY_STORE_SIZE][64];

  struct discord_emoji emojis[4];
  char emoji_names[BUNNY_STORE_SIZE][64];

  struct discord_embed_field fields[BUNNY_STORE_SIZE +2]; // +2 offset for balance and resources
  char field_names[BUNNY_STORE_SIZE +2][64];
  char field_values[BUNNY_STORE_SIZE +2][512];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

void init_bunny_fields(struct sd_bunny_shop *params, struct sd_player *player, struct sd_store *bunny_shop)
{
  APPLY_NUM_STR(catnip, player->catnip);

  params->fields[0] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Balance"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        ""INDENT" "CATNIP" Catnip: **%s**", catnip)
  };

  APPLY_NUM_STR(acorns, player->acorns);
  APPLY_NUM_STR(golden_acorns, player->golden_acorns);
  APPLY_NUM_STR(health, player->health);
  APPLY_NUM_STR(max_health, player->max_health);

  params->fields[1] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Resources"),
    .value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
        ""INDENT" "ACORNS" Acorns: **%s** \n"
        ""INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
        ""INDENT" "HEALTH" Health: **%s**/**%s** \n"
        ""INDENT" "ENERGY" Energy: **%d**/%d \n",
        acorns, golden_acorns, health, max_health, player->energy, MAX_ENERGY)
  };

  for (int field_idx = 2; field_idx < BUNNY_STORE_SIZE +2; field_idx++)
  {
    APPLY_NUM_STR(quantity, bunny_shop[field_idx -2].quantity);
    APPLY_NUM_STR(item_cost, bunny_shop[field_idx -2].cost);

    struct sd_file_data item = items[bunny_shop[field_idx -2].item_idx];

    params->fields[field_idx] = (struct discord_embed_field) {
      .name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), "%s <:%s:%ld> %s",
          quantity, item.emoji_name, item.emoji_id, item.formal_name),
      .value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
          ""INDENT" *Requires* **%s** "CATNIP" Catnip", item_cost)
    };
  }
}

void init_bunny_buttons(const struct discord_interaction *event, struct sd_bunny_shop *params, struct sd_player *player, struct sd_store *bunny_shop)
{
  for (int button_idx = 0; button_idx < BUNNY_STORE_SIZE; button_idx++) 
  {
    struct sd_file_data item = items[bunny_shop[button_idx].item_idx];

    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                item.emoji_name),
        .id = item.emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), item.formal_name),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_BUNNY, button_idx, ERROR_STATUS + 96, event->member->user->id)
    };

    // hash out exceptions first
    if (button_idx == BUNNY_HEALTH
      && player->max_health - player->health < bunny_shop[BUNNY_HEALTH].quantity)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
    else if (button_idx == BUNNY_ENERGY 
      && MAX_ENERGY - player->energy < bunny_shop[BUNNY_ENERGY].quantity)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
    // then compare catnip as usual
    else if (player->catnip >= bunny_shop[button_idx].cost)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
  }
}

void bunny_cmd_state(const struct discord_interaction *event, struct sd_bunny_shop *params, struct sd_player *player, struct sd_store *bunny_shop)
{
  int* stat_ptr[BUNNY_STORE_SIZE] = {
    &player->acorns, &player->golden_acorns, &player->health, &player->energy
  };

  if (event->data->custom_id)
  {
    int button_idx = event->data->custom_id[1] -48;

    if (player->catnip < bunny_shop[button_idx].cost)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You need more catnip!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else if (button_idx == BUNNY_HEALTH
      && player->max_health - player->health < bunny_shop[BUNNY_HEALTH].quantity)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You have too much health!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else if (button_idx == BUNNY_ENERGY 
      && MAX_ENERGY - player->energy < bunny_shop[BUNNY_ENERGY].quantity)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You have too much energy!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else {
      for (int item_idx = 0; item_idx < BUNNY_STORE_SIZE; item_idx++)
      {
        if (button_idx == item_idx)
        {
          APPLY_NUM_STR(quantity, bunny_shop[button_idx].quantity);
          (*stat_ptr[button_idx]) += bunny_shop[button_idx].quantity;
          player->catnip -= bunny_shop[button_idx].cost;
          struct sd_file_data item = items[bunny_shop[button_idx].item_idx];
          u_snprintf(params->footer_text, sizeof(params->footer_text), "You received %s %s!", quantity, item.formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item.file_path);
          break;
        }
      }
      // update player only on selection
      update_player_row(player);
    }
  }
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Bunny's Wares!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

int bunny_interaction(const struct discord_interaction *event)
{
  struct tm *info = get_UTC();
  ERROR_INTERACTION((info->tm_mday < 21), "This event is not active!");
  
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  energy_regen(&player);

  struct sd_bunny_shop params = { 0 };

  struct sd_store bunny_shop[BUNNY_STORE_SIZE] =
  {
    {
      .item_idx = ITEM_ACORNS,
      .cost = 500,
      .quantity = PROFICIENCY_UNIT * (player.stats.proficiency_lv +1),
    },
    {
      .item_idx = ITEM_GOLDEN_ACORN,
      .cost = 500,
      .quantity = PROFICIENCY_UNIT/2 * (player.stats.luck_lv +1),
    },
    {
      .item_idx = ITEM_HEALTH,
      .cost = 500,
      .quantity = STRENGTH_FACTOR/2 * (player.stats.strength_lv),
    },
    {
      .item_idx = ITEM_ENERGY,
      .cost = 500,
      .quantity = MAX_ENERGY/2
    }
  };

  bunny_cmd_state(event, &params, &player, bunny_shop);

  init_bunny_buttons(event, &params, &player, bunny_shop);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = BUNNY_STORE_SIZE
    }
  };

  init_bunny_fields(&params, &player, bunny_shop);

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
    .title = u_snprintf(header.title, sizeof(header.title), "Bunny's Wares"),
    .description = u_snprintf(params.description, sizeof(params.description), 
        ""OFF_ARROW" Purchase a resource with "CATNIP" *catnip*! \n"
        ""OFF_ARROW" All quantities increase with your stats except "ENERGY" *energy*."),
    
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, BUNNY_ED_PATH)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = BUNNY_STORE_SIZE +2
    },
    .footer = &(struct discord_embed_footer) {
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

  return 0;
}