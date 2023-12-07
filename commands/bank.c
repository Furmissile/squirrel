struct sd_bank
{
  struct discord_component deposit_buttons[3];
  char deposit_custom_ids[3][64];
  char deposit_labels[3][64];

  struct discord_component withdrawal_buttons[3];
  char withdrawal_custom_ids[3][64];
  char withdrawal_labels[3][64];

  struct discord_emoji emojis[2];
  char emoji_names[2][64];

  struct discord_embed_field field; // for balances
  char field_name[64];
  char field_value[512];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

void init_bank_fields(struct sd_bank *params, struct sd_player *player)
{
  APPLY_NUM_STR(golden_acorns, player->golden_acorns);
  APPLY_NUM_STR(stored_golden_acorns, player->stored_golden_acorns);

  params->field = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_name, sizeof(params->field_name), "Balance"),
    .value = u_snprintf(params->field_value, sizeof(params->field_value),
        " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
        " "INDENT" "LOST_STASH" Stored Golden Acorns: **%s** \n",
        golden_acorns, stored_golden_acorns)
  };
}

void init_bank_buttons(const struct discord_interaction *event, struct sd_bank *params, struct sd_player *player)
{
  char amount_labels[3][64] = { "1,000", "10,000", "100,000" };
  int amounts[3] = { 1000, 10000, 100000 };

  params->emojis[0] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[0], sizeof(params->emoji_names[0]), 
            misc[MISC_ADD].emoji_name),
    .id = misc[MISC_ADD].emoji_id
  };
  params->emojis[1] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[1], sizeof(params->emoji_names[1]), 
            misc[MISC_SUBTRACT].emoji_name),
    .id = misc[MISC_SUBTRACT].emoji_id
  };

  int button_idx = 0;

  // deposit buttons
  for (; button_idx < 3; button_idx++)
  {
    params->deposit_buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[0],
      .label = u_snprintf(params->deposit_labels[button_idx], sizeof(params->deposit_labels[button_idx]), amount_labels[button_idx]),
      .custom_id = u_snprintf(params->deposit_custom_ids[button_idx], sizeof(params->deposit_custom_ids[button_idx]), "%c%d *%d* .%ld",
          TYPE_BANK, button_idx, amounts[button_idx], event->member->user->id)
    };

    if (player->golden_acorns >= amounts[button_idx])
    {
      params->deposit_buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
    else {
      params->deposit_buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->deposit_buttons[button_idx].disabled = true;
    }
  }

  // withdrawal buttons
  for (;button_idx < 6; button_idx++)
  {
    int amount_idx = button_idx -3;
    params->withdrawal_buttons[amount_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[1],
      .label = u_snprintf(params->withdrawal_labels[amount_idx], sizeof(params->withdrawal_labels[amount_idx]), amount_labels[amount_idx]),
      .custom_id = u_snprintf(params->withdrawal_custom_ids[amount_idx], sizeof(params->withdrawal_custom_ids[amount_idx]), "%c%d *%d* .%ld",
          TYPE_BANK, button_idx, amounts[amount_idx], event->member->user->id)
    };

    if (player->stored_golden_acorns >= amounts[amount_idx])
    {
      params->withdrawal_buttons[amount_idx].style = DISCORD_BUTTON_PRIMARY;
    }
    else {
      params->withdrawal_buttons[amount_idx].style = DISCORD_BUTTON_SECONDARY;
      params->withdrawal_buttons[amount_idx].disabled = true;
    }
  }

}

void bank_cmd_state(const struct discord_interaction *event, struct sd_bank *params, struct sd_player *player)
{
  // this is the first row
  if (player->button_idx/3 == 0)
  {
    char quantity[64] = { };
    trim_buffer(quantity, sizeof(quantity), event->data->custom_id, '*');
    int amount = strtoint(quantity);

    if (player->golden_acorns < amount)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You don't have enough golden acorns!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
      return;
    }

    player->golden_acorns -= amount;
    player->stored_golden_acorns += amount;

    APPLY_NUM_STR(golden_acorns, amount);
    u_snprintf(params->footer_text, sizeof(params->footer_text), "You have deposited %s golden acorns!", golden_acorns);
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, items[ITEM_GOLDEN_ACORN].file_path);  
  }
  // this is the second row
  else if (player->button_idx/3 == 1)
  {
    char quantity[64] = { };
    trim_buffer(quantity, sizeof(quantity), event->data->custom_id, '*');
    int amount = strtoint(quantity);

    if (player->stored_golden_acorns < amount)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You don't have enough golden acorns!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
      return;
    }

    player->stored_golden_acorns -= amount;
    player->golden_acorns += amount;

    APPLY_NUM_STR(golden_acorns, amount);
    u_snprintf(params->footer_text, sizeof(params->footer_text), "You have withdrawn %s golden acorns!", golden_acorns);
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, items[ITEM_GOLDEN_ACORN].file_path);  
  }
  // this came from somewhere else
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Bank!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, items[ITEM_GOLDEN_ACORN].file_path);
  }
}

int bank_interaction(const struct discord_interaction *event) 
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_header_params header = { 0 };
  struct sd_bank params = { 0 };

  bank_cmd_state(event, &params, &player);

  init_bank_fields(&params, &player);
  init_bank_buttons(event, &params, &player);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Bank"),
    .description = u_snprintf(params.description, sizeof(params.description),
        " "OFF_ARROW" "ADD" is for depositing and "MINUS" is for withdrawing "GOLDEN_ACORNS" *golden acorns*. \n"
        " "OFF_ARROW" The golden acorns stored here are never drawn from."),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          (player.designer_squirrel == ERROR_STATUS) 
            ? squirrels[player.squirrel].squirrel.file_path 
            : designer_squirrels[player.designer_squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields)
    {
      .array = &params.field,
      .size = 1
    },
    .footer = &(struct discord_embed_footer) {
      .text = params.footer_text,
      .icon_url = params.footer_url
    }
  };

  struct sd_secondary secondary = { 0 };

  generate_secondary_buttons(event, &secondary, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[4] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.deposit_buttons,
        .size = 3
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.withdrawal_buttons,
        .size = 3
      }
    },
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
        .size = 4
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