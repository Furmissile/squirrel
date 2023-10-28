struct sd_squirrel_shop {
  struct discord_component buttons[SQUIRREL_SIZE +1];
  char custom_ids[SQUIRREL_SIZE +1][64]; // +1 for refresh button
  char labels[SQUIRREL_SIZE +1][64];

  struct discord_emoji emojis[SQUIRREL_SIZE +1];
  char emoji_names[SQUIRREL_SIZE +1][64];

  struct discord_embed_field fields[SQUIRREL_SIZE +1]; // +1 offset for score field
  char field_names[SQUIRREL_SIZE +1][64];
  char field_values[SQUIRREL_SIZE +1][256];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

void init_squirrel_fields(struct sd_squirrel_shop *params, struct sd_player *player)
{
  APPLY_NUM_STR(acorn_count, player->acorn_count);

  params->fields[0] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Current Score"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        ""INDENT" "ACORN_COUNT" Acorn Count: **%s**", acorn_count)
  };
  
  for (int field_idx = 1; field_idx < SQUIRREL_SIZE +1; field_idx++)
  {
    struct sd_file_data squirrel_data = squirrels[field_idx -1].squirrel;

    APPLY_NUM_STR(squirrel_req, squirrels[field_idx -1].acorn_count_req);

    params->fields[field_idx] = (struct discord_embed_field) {
      .name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), 
          "<:%s:%ld> %s",
          squirrel_data.emoji_name, squirrel_data.emoji_id, squirrel_data.formal_name),
      .value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
          " "OFF_ARROW" %s \n"
          " *Requires* **%s** "ACORN_COUNT" Acorn Count",
          squirrel_data.description, squirrel_req)
    };

  }
}

void init_squirrel_buttons(const struct discord_interaction *event, struct sd_squirrel_shop *params, struct sd_player *player)
{
  for (int button_idx = 0; button_idx < SQUIRREL_SIZE; button_idx++) 
  {
    struct sd_file_data squirrel_data = squirrels[button_idx].squirrel;

    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                squirrel_data.emoji_name),
        .id = squirrel_data.emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), squirrel_data.formal_name),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_SQUIRREL, button_idx, ERROR_STATUS + 96, event->member->user->id)
    };

    if (player->buffs.boosted_acorn > 0 
      && button_idx != player->squirrel)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
    else if (player->acorn_count >= squirrels[button_idx].acorn_count_req
      && button_idx != player->squirrel)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
    else if (button_idx == player->squirrel)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SUCCESS;
      params->buttons[button_idx].disabled = true;
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
  }
}

void squirrel_cmd_state(const struct discord_interaction *event, struct sd_squirrel_shop *params, struct sd_player *player)
{
  if (event->data->custom_id && player->button_idx < SQUIRREL_SIZE)
  {
    int button_idx = player->button_idx;
    if (player->acorn_count < squirrels[button_idx].acorn_count_req)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You need a higher acorn count!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
    }
    else {
      for (int squirrel_idx = 0; squirrel_idx < SQUIRREL_SIZE; squirrel_idx++)
      {
        if (button_idx == squirrel_idx)
        {
          player->squirrel = squirrel_idx;
          u_snprintf(params->footer_text, sizeof(params->footer_text), "You selected the %s!", squirrels[squirrel_idx].squirrel.formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, squirrels[squirrel_idx].squirrel.file_path);
          break;
        }
      }
      // update player only on selection
      update_player_row(player);
    }
  }
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Squirrel Shop!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

int squirrels_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event);
  player.button_idx = (event->data->custom_id) ? event->data->custom_id[1] -48 : ERROR_STATUS;
  
  struct sd_squirrel_shop params = { 0 };

  squirrel_cmd_state(event, &params, &player);

  init_squirrel_buttons(event, &params, &player);
  init_squirrel_fields(&params, &player);

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
    .title = u_snprintf(header.title, sizeof(header.title), "Squirrel Shop"),
    .description = u_snprintf(params.description, sizeof(params.description), 
        " "OFF_ARROW" Squirrels are unlocked based on your current "ACORN_COUNT" *acorn count*! \n"
        " "OFF_ARROW" Upon death, your squirrel is reset if your new acorn count doesn't make the requirement. \n"
        " "OFF_ARROW" If the "BOOSTED_ACORN" *boosted acorn* is active, changing squirrel is not possible until it has depleted."),
    
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = SQUIRREL_SIZE +1
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
        .size = SQUIRREL_SIZE
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
