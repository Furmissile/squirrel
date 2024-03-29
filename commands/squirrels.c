enum SQ_TOPIC {
  SQ_TOPIC_TRADITIONAL,
  SQ_TOPIC_SPRING,
  SQ_TOPIC_SUMMER,
  SQ_TOPIC_FALL,
  SQ_TOPIC_WINTER,
  SQ_TOPIC_SIZE
};

struct sd_squirrrel_info
{
  struct discord_component buttons[4];
  char custom_ids[4][64];
  char labels[4][64];

  struct discord_emoji emojis[4];
  char emoji_names[4][64];

  struct discord_component page_buttons[5];
  char page_custom_ids[5][64];

  struct discord_emoji reset_emoji;
  char reset_emoji_name[64];

  struct discord_embed_field fields[4 +1]; // +1 offset for balance field
  char field_names[4 +1][128];
  char field_values[4 +1][256];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};


void init_squirrel_fields(struct sd_squirrrel_info *params, struct sd_player *player, int page_idx)
{
  APPLY_NUM_STR(conjured_acorns, player->conjured_acorns);

  params->fields[0] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Balance"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        ""INDENT" "CONJURED_ACORNS" Conjured Acorns: **%s**", conjured_acorns)
  };

  for (int field_idx = 1; field_idx < 4 +1; field_idx++)
  {
    int squirrel_idx = 4 * page_idx + (field_idx -1) +1;
    struct sd_file_data squirrel_data = squirrels[squirrel_idx].squirrel;

    params->fields[field_idx] = (struct discord_embed_field) { 0 };

    params->fields[field_idx].name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), 
        "<:%s:%ld> %s",
        squirrel_data.emoji_name, squirrel_data.emoji_id, squirrel_data.formal_name);

    if ((player->purchased >> squirrel_idx & 1) == 0)
    {
      APPLY_NUM_STR(squirrel_req, squirrels[squirrel_idx].cost);
      params->fields[field_idx].value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
          " "OFF_ARROW" %s \n"
          " "INDENT" *Costs* **%s** "CONJURED_ACORNS" Conjured Acorns", 
          squirrel_data.description, squirrel_req);
    }
    else {
      params->fields[field_idx].value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
          " "OFF_ARROW" %s \n"
          " "INDENT" "QUEST_MARKER" *Purchased*", 
          squirrel_data.description);
    }

  }
}

void init_squirrel_buttons(const struct discord_interaction *event, struct sd_squirrrel_info *params, struct sd_player *player, int page_idx)
{
  for (int button_idx = 0; button_idx < 4; button_idx++) 
  {
    int squirrel_idx = 4 * page_idx + button_idx +1; // +1 for gray squirrel
    struct sd_file_data squirrel_data = squirrels[squirrel_idx].squirrel;

    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                squirrel_data.emoji_name),
        .id = squirrel_data.emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%d.%ld",
                    TYPE_SQUIRREL, button_idx, page_idx, event->member->user->id)
    };

    // if player has yet to purchase the squirrel
    if ((player->purchased >> squirrel_idx & 1) == 0)
    {
      if (player->conjured_acorns < squirrels[squirrel_idx].cost)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
        params->buttons[button_idx].disabled = true;
      }
      else {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
    
      params->buttons[button_idx].label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), "Purchase");
    }
    // if player has equipt the current squirrel
    else if (squirrel_idx == player->squirrel)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SUCCESS;
      params->buttons[button_idx].disabled = true;
      params->buttons[button_idx].label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), squirrel_data.formal_name);
    }
    // if player has squirrel and squirrel is not equipt
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      params->buttons[button_idx].label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), squirrel_data.formal_name);
    }
  }
}

void init_squirrels_page_buttons(const struct discord_interaction *event, struct sd_squirrrel_info *params, struct sd_player *player, int page_idx)
{
  params->page_buttons[0] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏮️",
    .custom_id = u_snprintf(params->page_custom_ids[0], sizeof(params->page_custom_ids[0]), 
        "%c00.%ld", TYPE_SQUIRREL_HELP, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->page_buttons[1] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏪",
    .custom_id = u_snprintf(params->page_custom_ids[1], sizeof(params->page_custom_ids[1]), 
        "%c1%d.%ld", TYPE_SQUIRREL_HELP, (page_idx > 0) ? page_idx -1 : 0, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->reset_emoji = (struct discord_emoji) {
    .name = u_snprintf(params->reset_emoji_name, sizeof(params->reset_emoji_name), misc[MISC_HELP].emoji_name),
    .id = misc[MISC_HELP].emoji_id
  };

  params->page_buttons[2] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .emoji = &params->reset_emoji,
    .label = "Reset Squirrel",
    .custom_id = u_snprintf(params->page_custom_ids[2], sizeof(params->page_custom_ids[2]), 
        "%c2%d.%ld", TYPE_SQUIRREL_HELP, page_idx, event->member->user->id),
  };

  if (player->squirrel > 0)
  {
    params->page_buttons[2].style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    params->page_buttons[2].style = DISCORD_BUTTON_SECONDARY;
    params->page_buttons[2].disabled = true;  
  }

  params->page_buttons[3] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏩",
    .custom_id = u_snprintf(params->page_custom_ids[3], sizeof(params->page_custom_ids[3]), 
        "%c3%d.%ld", TYPE_SQUIRREL_HELP, (page_idx < SQ_TOPIC_SIZE) ? page_idx +1 : SQ_TOPIC_SIZE -1, event->member->user->id),
    .style = (page_idx < SQ_TOPIC_SIZE -1) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == SQ_TOPIC_SIZE -1) ? true : false
  };

  params->page_buttons[4] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏭️",
    .custom_id = u_snprintf(params->page_custom_ids[4], sizeof(params->page_custom_ids[4]),
        "%c4%d.%ld", TYPE_SQUIRREL_HELP, SQ_TOPIC_SIZE -1, event->member->user->id),
    .style = (page_idx < SQ_TOPIC_SIZE -1) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == SQ_TOPIC_SIZE -1) ? true : false
  };
}

void squirrels_cmd_state(const struct discord_interaction *event, struct sd_squirrrel_info *params, struct sd_player *player, int page_idx)
{
  // [2] because [1] is page number!
  if (event->data->custom_id[0] == TYPE_SQUIRREL_HELP && player->button_idx == 2)
  {
    player->squirrel = GRAY_SQUIRREL;
    struct sd_file_data squirrel_data = squirrels[player->squirrel].squirrel;

    u_snprintf(params->footer_text, sizeof(params->footer_text), "Your squirrel has been reset!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, squirrel_data.file_path);
  }
  else if (event->data->custom_id[0] == TYPE_SQUIRREL
    && player->button_idx < 4) 
  {
    int squirrel_idx = 4 * page_idx + player->button_idx +1;
    for (int button_idx = 0; button_idx < 4; button_idx++)
    {
      if (button_idx == player->button_idx)
      {
        struct sd_file_data squirrel_data = squirrels[squirrel_idx].squirrel;

        if ((player->purchased >> squirrel_idx & 1) == 0)
        {
          player->purchased |= (1 << squirrel_idx);
          player->conjured_acorns -= squirrels[squirrel_idx].cost;

          u_snprintf(params->footer_text, sizeof(params->footer_text), "You have purchased the %s!", squirrel_data.formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, squirrel_data.file_path);
        }
        // otherwise the player is selecting the squirrel!
        else {
          player->squirrel = squirrel_idx;
          u_snprintf(params->footer_text, sizeof(params->footer_text), "You have selected the %s!", squirrel_data.formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, squirrel_data.file_path);
        }

        break;
      }
    }
  }
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Designer Squirrels Shop!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, misc[MISC_ALERT].file_path);
  }

}

int squirrels_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  ERROR_INTERACTION((event->message->timestamp /1000 < player.timestamp), 
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp /1000;

  char* seasons[5] = {"Traditional", "Spring", "Summer", "Fall", "Winter"};
  int page_num = event->data->custom_id[2] -48 +1;
  
  struct sd_squirrrel_info params = { 0 };

  squirrels_cmd_state(event, &params, &player, page_num -1);

  init_squirrel_buttons(event, &params, &player, page_num -1);
  init_squirrels_page_buttons(event, &params, &player, page_num -1);
  init_squirrel_fields(&params, &player, page_num -1);

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Designer Squirrels (%s List)", seasons[page_num -1]),
    .description = u_snprintf(params.description, sizeof(params.description), 
        " "OFF_ARROW" Purchase a squirrel custom with "CONJURED_ACORNS" *conjured acorns*! \n"
        " "OFF_ARROW" If you lose your squirrel from death, the custom will still be active. \n"
        " "OFF_ARROW" The appearance of your squirrel is cosmetic."),
    
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, 
          squirrels[player.squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 4 +1
    },
    .footer = &(struct discord_embed_footer) {
      .text = params.footer_text,
      .icon_url = params.footer_url
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = 4
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.page_buttons,
        .size = 5
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = util_data.buttons_displayed
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
        .size = 3
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);

  return 0;
}
