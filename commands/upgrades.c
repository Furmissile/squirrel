struct sd_upgrade_shop {
  struct discord_component buttons[STAT_SIZE];
  char custom_ids[STAT_SIZE +1][64]; // +1 for refresh button
  char labels[STAT_SIZE +1][64];

  struct discord_emoji emojis[STAT_SIZE];
  char emoji_names[STAT_SIZE][64];

  struct discord_embed_field fields[STAT_SIZE];
  char field_names[STAT_SIZE][64];
  char field_values[STAT_SIZE][256];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

void init_upgrade_fields(struct sd_upgrade_shop *params, struct sd_player *player)
{
  int* stat_ptrs[3] = { &player->stats.proficiency_lv, &player->stats.luck_lv, &player->stats.strength_lv };

  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    struct sd_file_data stat_data = stats[button_idx].stat;

    APPLY_NUM_STR(stat_cost, generate_price(*stat_ptrs[button_idx], stats[button_idx].price_mult));

    params->fields[button_idx] = (struct discord_embed_field) { 0 };

    params->fields[button_idx].name = u_snprintf(params->field_names[button_idx], sizeof(params->field_names[button_idx]), " <:%s:%ld> %s (Lv. %d)",
        stat_data.emoji_name, stat_data.emoji_id, stat_data.formal_name, *stat_ptrs[button_idx]);

    u_snprintf(params->field_values[button_idx], sizeof(params->field_values[button_idx]), 
        (button_idx == STAT_STRENGTH) 
            ? " "OFF_ARROW" %s (+**%0.0f**) \n" " *Costs* **%s** "ACORNS" Acorns"
            : " "OFF_ARROW" %s (x**%0.1f**) \n" " *Costs* **%s** "ACORNS" Acorns",
        stat_data.description, generate_factor(*stat_ptrs[button_idx], stats[button_idx].value_mult), stat_cost);

    if (button_idx == STAT_LUCK && player->stats.proficiency_lv < player->stats.luck_lv * STAT_DIFFERENCE)
    {
      u_snprintf(params->field_values[button_idx], sizeof(params->field_values[button_idx]),
          "\n "HELP_MARKER" Requires level **%d** proficiency to upgrade!",
          player->stats.luck_lv *STAT_DIFFERENCE);
    }
    else if (button_idx == STAT_STRENGTH && player->stats.luck_lv < player->stats.strength_lv * STAT_DIFFERENCE)
    {
      u_snprintf(params->field_values[button_idx], sizeof(params->field_values[button_idx]),
          "\n "HELP_MARKER" Requires level **%d** luck to upgrade!",
          player->stats.strength_lv *STAT_DIFFERENCE);
    }

    params->fields[button_idx].value = params->field_values[button_idx];
  }
}

void init_upgrade_buttons(const struct discord_interaction *event, struct sd_upgrade_shop *params, struct sd_player *player)
{
  int* stat_ptrs[STAT_SIZE] = { &player->stats.proficiency_lv, &player->stats.luck_lv, &player->stats.strength_lv };

  for (int button_idx = 0; button_idx < STAT_SIZE; button_idx++) 
  {    
    struct sd_file_data stat_data = stats[button_idx].stat;
    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                stat_data.emoji_name),
        .id = stat_data.emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), 
          ((*stat_ptrs[button_idx] +1) % STAT_EVOLUTION == 0) ? "EVOLVE" : stat_data.formal_name),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_UPGRADE, button_idx, ERROR_STATUS + 96, event->member->user->id)
    };

    int price = generate_price(*stat_ptrs[button_idx], stats[button_idx].price_mult);

    if (player->acorns >= price)
    {
      if (button_idx == STAT_PROFICIENCY)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
      else if (button_idx == STAT_LUCK && player->stats.proficiency_lv >= player->stats.luck_lv * STAT_DIFFERENCE)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
      else if (button_idx == STAT_STRENGTH && player->stats.luck_lv >= player->stats.strength_lv * STAT_DIFFERENCE)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
      else {
        params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
        params->buttons[button_idx].disabled = true;
      }
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }

    // BETA testing
    // if (player->acorns >= generate_price(*stat_ptrs[button_idx], stats[button_idx].price_mult))
    // {
    //   params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    // }
    // else {
    //   params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
    //   params->buttons[button_idx].disabled = true;
    // }
  }
}

void upgrade_command_state(const struct discord_interaction *event, struct sd_upgrade_shop *params, struct sd_player *player)
{
  if (event->data->custom_id && player->button_idx < STAT_SIZE) 
  {
    int button_idx = player->button_idx;
    int* stat_ptrs[STAT_SIZE] = { &player->stats.proficiency_lv, &player->stats.luck_lv, &player->stats.strength_lv };
  
    if (player->acorns < generate_price(*(stat_ptrs[button_idx]), stats[button_idx].price_mult))
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You need more acorns to make this upgrade!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path); 
    }
    else {
      for (int stat_idx = 0; stat_idx < STAT_SIZE; stat_idx++) 
      {
        if (button_idx == stat_idx) {
          player->acorns -= generate_price(*(stat_ptrs[stat_idx]), stats[stat_idx].price_mult);
          (*stat_ptrs[stat_idx])++;

          struct sd_file_data stat_data = stats[stat_idx].stat;
          u_snprintf(params->footer_text, sizeof(params->footer_text), "You received the stat of %s", stat_data.formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, stat_data.file_path);
          
          if (stat_idx == STAT_STRENGTH)
            player->health += STRENGTH_FACTOR;
          
          break;
        }
      }
      // update player only on purchase
      update_player_row(player);
    }
  }
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Upgrades Shop!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

int init_upgrade_shop(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event);

  struct sd_upgrade_shop params = { 0 };

  upgrade_command_state(event, &params, &player);

  init_upgrade_buttons(event, &params, &player);
  init_upgrade_fields(&params, &player);

  APPLY_NUM_STR(acorns, player.acorns);
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
    .title = u_snprintf(header.title, sizeof(header.title), "Upgrades Shop"),
    .description = u_snprintf(params.description, sizeof(params.description), 
        "**Balance**: \n "INDENT" "ACORNS" Acorns: **%s**", acorns),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = STAT_SIZE
    },
    .footer = &(struct discord_embed_footer) {
      .text = params.footer_text,
      .icon_url = params.footer_url
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = STAT_SIZE
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
        .size = 2
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}
