struct sd_buffs_shop {
  struct discord_component buttons[BUFFS_SIZE];
  char custom_ids[BUFFS_SIZE +1][64]; // +1 for refresh button
  char labels[BUFFS_SIZE +1][64];

  struct discord_emoji emojis[BUFFS_SIZE];
  char emoji_names[BUFFS_SIZE][64];

  struct discord_embed_field fields[BUFFS_SIZE +2]; // +2 for balance and cost
  char field_names[BUFFS_SIZE +2][128];
  char field_values[BUFFS_SIZE +2][256];

  char thumbnail_url[128];
  char description[512];

  char footer_text[64];
  char footer_url[128];
};

void init_buffs_fields(struct sd_buffs_shop *params, struct sd_player *player)
{
  APPLY_NUM_STR(golden_acorns, player->golden_acorns);
  APPLY_NUM_STR(conjured_acorns, player->conjured_acorns);

  params->fields[0] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Balance"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
        " "INDENT" "CONJURED_ACORNS" Conjured Acorns: **%s**",
        golden_acorns, conjured_acorns)
  };

  APPLY_NUM_STR(buff_cost, GOLDEN_ACORN_BUFF_COST * generate_factor(player->stats.luck_lv, LUCK_FACTOR));
  params->fields[1] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Enchanted Acorn Cost"),
    .value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), 
        " "INDENT" **%s** "GOLDEN_ACORNS" Golden Acorns \n"
        " "INDENT" **%d** "CONJURED_ACORNS" Conjured Acorns",
        buff_cost, SQUIRREL_BOOST_COST)
  };

  int* stat_ptrs[5] = { 
    &player->buffs.proficiency_acorn, 
    &player->buffs.luck_acorn, 
    &player->buffs.strength_acorn, 
    &player->buffs.endurance_acorn, 
    &player->buffs.boosted_acorn 
  };

  for (int field_idx = 0 +2; field_idx < BUFFS_SIZE +2; field_idx++)
  {
    struct sd_file_data e_acorn_data = enchanted_acorns[field_idx -2];
    APPLY_NUM_STR(buff_count, *stat_ptrs[field_idx -2]);

    if (*stat_ptrs[field_idx -2] > 0)
      params->fields[field_idx] = (struct discord_embed_field) {
        .name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), 
            "<:%s:%ld> %s ("QUEST_MARKER" **%s**)", 
            e_acorn_data.emoji_name, e_acorn_data.emoji_id, e_acorn_data.formal_name, buff_count),
        .value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
            " "OFF_ARROW" %s", e_acorn_data.description)
      };
    else {
      params->fields[field_idx] = (struct discord_embed_field) {
        .name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), 
            "<:%s:%ld> %s", 
            e_acorn_data.emoji_name, e_acorn_data.emoji_id, e_acorn_data.formal_name, buff_count),
        .value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), 
            " "OFF_ARROW" %s", e_acorn_data.description)
      };
    }
  }
  
}

void init_buffs_buttons(const struct discord_interaction *event, struct sd_buffs_shop *params, struct sd_player *player)
{
  for (int button_idx = 0; button_idx < BUFFS_SIZE; button_idx++)
  {
    struct sd_file_data e_acorn_data = enchanted_acorns[button_idx];

    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                e_acorn_data.emoji_name),
        .id = e_acorn_data.emoji_id
    };
    
    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = &params->emojis[button_idx],
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), e_acorn_data.formal_name),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d_%ld",
                    TYPE_E_ACORN, button_idx, event->member->user->id)
    };

    if (button_idx == BUFF_BOOSTED_ACORN)
    {
      if (player->conjured_acorns >= SQUIRREL_BOOST_COST)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
      else {
        params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
        params->buttons[button_idx].disabled = true;
      }
    }
    else {
      if (button_idx == BUFF_STRENGTH_ACORN && player->health >= player->max_health)
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
        params->buttons[button_idx].disabled = true;
      }
      else if (player->golden_acorns >= GOLDEN_ACORN_BUFF_COST * generate_factor(player->stats.luck_lv, LUCK_FACTOR))
      {
        params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
      }
      else {
        params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
        params->buttons[button_idx].disabled = true;
      }
    }
  }

}

void buffs_command_state(const struct discord_interaction *event, struct sd_buffs_shop *params, struct sd_player *player)
{
  if (event->data->custom_id && event->data->custom_id[1] -48 < BUFFS_SIZE) 
  {
    int* stat_ptrs[5] = { 
      &player->buffs.proficiency_acorn, 
      &player->buffs.luck_acorn, 
      &player->buffs.strength_acorn, 
      &player->buffs.endurance_acorn, 
      &player->buffs.boosted_acorn 
    };

    int button_idx = event->data->custom_id[1] -48;

    // if an enchanted acorn and not enough golden acorn
    if (button_idx != BUFF_BOOSTED_ACORN && player->golden_acorns < GOLDEN_ACORN_BUFF_COST * generate_factor(player->stats.luck_lv, LUCK_FACTOR))
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You need more golden acorns!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    // or the boosted acorn and not enough conjured acorn
    else if (button_idx == BUFF_BOOSTED_ACORN && player->conjured_acorns < SQUIRREL_BOOST_COST)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You need more conjured acorns!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    // or the strength acorn with full health
    else if (button_idx == BUFF_STRENGTH_ACORN && player->health >= player->max_health)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "Your health is already full!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else {
      for (int buff_idx = 0; buff_idx < BUFFS_SIZE; buff_idx++) 
      {
        if (button_idx == buff_idx) // on match
        {
          if (button_idx == BUFF_BOOSTED_ACORN) // take conjured acorns if boosted acorn is bought
            player->conjured_acorns -= SQUIRREL_BOOST_COST;
          else 
            player->golden_acorns -= (GOLDEN_ACORN_BUFF_COST * generate_factor(player->stats.luck_lv, LUCK_FACTOR));

          if (button_idx == BUFF_STRENGTH_ACORN) // exception with strength acorn
          {
            int acorn_duration = genrand(15, 5) * player->stats.strength_lv;
            player->buffs.strength_acorn += acorn_duration;
            player->health += acorn_duration;
          }
          else {
            (*stat_ptrs[button_idx]) += genrand(15, 5);
          }

          u_snprintf(params->footer_text, sizeof(params->footer_text), "You received the %s!", enchanted_acorns[button_idx].formal_name);
          u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, enchanted_acorns[button_idx].file_path);
          update_player_row(player);
          break;
        }
      }
    }
  }
  else {
    // reset buff to only show what player bought in that moment
    player->buffs.strength_acorn = 0;
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to the Buffs Shop!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

int buffs_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  struct sd_buffs_shop params = { 0 };

  buffs_command_state(event, &params, &player);

  init_buffs_buttons(event, &params, &player);
  init_buffs_fields(&params, &player);

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
    .title = u_snprintf(header.title, sizeof(header.title), "Buffs Shop"),
    .description = u_snprintf(params.description, sizeof(params.description), 
        " "OFF_ARROW" Buffs are stackable and are only used when applicable. \n"
        " "OFF_ARROW" Active buffs show up with a "QUEST_MARKER" and its duration."),

    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, SQ_CHEM_PATH)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = BUFFS_SIZE +2
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
        .size = BUFFS_SIZE
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