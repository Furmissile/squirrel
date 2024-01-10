void generate_util_buttons(const struct discord_interaction *event, struct sd_player *player, struct sd_util_info *params)
{
  params->emojis[params->buttons_displayed] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[params->buttons_displayed], sizeof(params->emoji_names[params->buttons_displayed]), 
            slice_types[TYPE_ACORNS].item.emoji_name),
    .id = slice_types[TYPE_ACORNS].item.emoji_id
  };
  params->buttons[params->buttons_displayed] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = u_snprintf(params->labels[params->buttons_displayed], sizeof(params->labels[params->buttons_displayed]), "Forage"),
    .custom_id = u_snprintf(params->custom_ids[params->buttons_displayed], sizeof(params->custom_ids[params->buttons_displayed]), "%c%d.%ld",
        TYPE_ADD_PIECE, DETECT_START, event->member->user->id),
    .emoji = &params->emojis[params->buttons_displayed]
  };
  params->buttons_displayed++;

  params->emojis[params->buttons_displayed] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[params->buttons_displayed], sizeof(params->emoji_names[params->buttons_displayed]), 
            squirrels[player->squirrel].squirrel.emoji_name),
    .id = squirrels[player->squirrel].squirrel.emoji_id
  };
  struct tm *info = get_UTC();
  params->buttons[params->buttons_displayed] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[params->buttons_displayed], sizeof(params->labels[params->buttons_displayed]), "Squirrels"),
    .custom_id = u_snprintf(params->custom_ids[params->buttons_displayed], sizeof(params->custom_ids[params->buttons_displayed]), "%c5%d.%ld",
        TYPE_SQUIRREL, info->tm_mday/7 +1, event->member->user->id),
        // +1 for "Traditional" squirrel set
    .emoji = &params->emojis[params->buttons_displayed]
  };
  params->buttons_displayed++;

  struct sd_file_data biome_icon = biomes[player->biome].biome_icon;
  params->emojis[params->buttons_displayed] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[params->buttons_displayed], sizeof(params->emoji_names[params->buttons_displayed]), 
            biome_icon.emoji_name),
    .id = biome_icon.emoji_id
  };
  params->buttons[params->buttons_displayed] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[params->buttons_displayed], sizeof(params->labels[params->buttons_displayed]), "Info"),
    .custom_id = u_snprintf(params->custom_ids[params->buttons_displayed], sizeof(params->custom_ids[params->buttons_displayed]), "%c.%ld",
        TYPE_INFO_FROM_BUTTONS, event->member->user->id),
    .emoji = &params->emojis[params->buttons_displayed]
  };
  params->buttons_displayed++;

  params->emojis[params->buttons_displayed] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[params->buttons_displayed], sizeof(params->emoji_names[params->buttons_displayed]), 
            squirrels[SQUIRREL_BOOKIE].squirrel.emoji_name),
    .id = squirrels[SQUIRREL_BOOKIE].squirrel.emoji_id
  };
  
  params->buttons[params->buttons_displayed] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[params->buttons_displayed], sizeof(params->labels[params->buttons_displayed]), "Stealing"),
    .custom_id = u_snprintf(params->custom_ids[params->buttons_displayed], sizeof(params->custom_ids[params->buttons_displayed]), "%c.%ld",
        TYPE_INIT_STEAL, event->member->user->id),
    .emoji = &params->emojis[params->buttons_displayed]
  };
  params->buttons_displayed++;

  if (player->scurry_id > 0)
  {
    params->emojis[params->buttons_displayed] = (struct discord_emoji)
    {
      .name = u_snprintf(params->emoji_names[params->buttons_displayed], sizeof(params->emoji_names[params->buttons_displayed]), 
              misc[MISC_SCURRY_ICON].emoji_name),
      .id = misc[MISC_SCURRY_ICON].emoji_id
    };

    params->buttons[params->buttons_displayed] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_SECONDARY,
      .custom_id = u_snprintf(params->custom_ids[params->buttons_displayed], sizeof(params->custom_ids[params->buttons_displayed]), "%c%d*%ld*.%ld",
          TYPE_SCURRY_INFO, params->buttons_displayed, player->scurry_id, event->member->user->id),
      .label = u_snprintf(params->labels[params->buttons_displayed], sizeof(params->labels[params->buttons_displayed]), "Scurry Info"),
      .emoji = &params->emojis[params->buttons_displayed]
    };
    
    params->buttons_displayed++;
  }
}

void init_help_buttons(const struct discord_interaction *event, struct sd_help_buttons *params, struct sd_player *player)
{
  struct sd_file_data squirrel_dir = squirrels[player->squirrel].squirrel;
  params->emojis[0] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[0], sizeof(params->emoji_names[0]), 
            squirrel_dir.emoji_name),
    .id = squirrel_dir.emoji_id
  };
  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = u_snprintf(params->labels[0], sizeof(params->labels[0]), "Player"),
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]), "%c0.%ld",
        TYPE_PLAYER_HELP, event->member->user->id),
    .emoji = &params->emojis[0]
  };

  int squirrel_idx = rand() % SQUIRREL_SIZE;
  params->emojis[1] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[1], sizeof(params->emoji_names[1]), 
            squirrels[squirrel_idx].squirrel.emoji_name),
    .id = squirrels[squirrel_idx].squirrel.emoji_id
  };
  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = u_snprintf(params->labels[1], sizeof(params->labels[1]), "Event"),
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]), "%c0.%ld",
        TYPE_EVENT_HELP, event->member->user->id),
    .emoji = &params->emojis[1]
  };

  params->emojis[2] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[2], sizeof(params->emoji_names[2]), 
            misc[MISC_SCURRY_ICON].emoji_name),
    .id = misc[MISC_SCURRY_ICON].emoji_id
  };
  params->buttons[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = u_snprintf(params->labels[2], sizeof(params->labels[2]), "Scurry"),
    .custom_id = u_snprintf(params->custom_ids[2], sizeof(params->custom_ids[2]), "%c0.%ld",
        TYPE_SCURRY_HELP, event->member->user->id),
    .emoji = &params->emojis[2]
  };

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    if (player->button_idx == event->data->custom_id[1] -48
      && event->data->custom_id[0] == params->buttons[button_idx].custom_id[0])
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SUCCESS;
      params->buttons[button_idx].disabled = true;
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
  }
}

void init_page_buttons(const struct discord_interaction *event, struct sd_pages *params, int page_idx, char help_type, int last_topic)
{
  params->buttons[0] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏮️",
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]), 
        "%c0a.%ld", help_type, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->buttons[1] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏪",
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]), 
        "%c%db.%ld", help_type, (page_idx > 0) ? page_idx -1 : 0, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->buttons[2] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏩",
    .custom_id = u_snprintf(params->custom_ids[2], sizeof(params->custom_ids[2]), 
        "%c%dc.%ld", help_type, (page_idx < last_topic) ? page_idx +1 : last_topic, event->member->user->id),
    .style = (page_idx < last_topic) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == last_topic) ? true : false
  };

  params->buttons[3] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏭️",
    .custom_id = u_snprintf(params->custom_ids[3], sizeof(params->custom_ids[3]),
        "%c%dd.%ld", help_type, last_topic, event->member->user->id),
    .style = (page_idx < last_topic) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == last_topic) ? true : false
  };
}

void init_statistics_buttons(const struct discord_interaction *event, struct sd_statistics *params, struct sd_player *player)
{
  // button index for determining style is 2 instead of 1!!
  params->emojis[0] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[0], sizeof(params->emoji_names[0]), 
        misc[MISC_HELP].emoji_name),
    .id = misc[MISC_HELP].emoji_id
  };

  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]), "%c0.%ld",
        TYPE_PLAYER_HELP, event->member->user->id),
    .label = "Help",
    .emoji = &params->emojis[0]
  };

  params->emojis[1] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[1], sizeof(params->emoji_names[1]), 
            misc[MISC_ALERT].emoji_name),
    .id = misc[MISC_ALERT].emoji_id
  };

  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]), "%c%d1.%ld",
        TYPE_BIOME_STORY, player->biome, event->member->user->id),
    .label = u_snprintf(params->labels[1], sizeof(params->labels[1]), "Biome Story"),
    .emoji = &params->emojis[1]
  };

  params->emojis[2] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[2], sizeof(params->emoji_names[2]), 
            misc[MISC_CROWN].emoji_name),
    .id = misc[MISC_CROWN].emoji_id
  };

  params->buttons[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = u_snprintf(params->custom_ids[2], sizeof(params->custom_ids[2]), "%c02.%ld",
        TYPE_LEADERBOARD, event->member->user->id),
    .label = u_snprintf(params->labels[2], sizeof(params->labels[2]), "Leaderboard"),
    .emoji = &params->emojis[2]
  };

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    if (player->button_idx == event->data->custom_id[1] -48
      && event->data->custom_id[0] == params->buttons[button_idx].custom_id[0])
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SUCCESS;
      params->buttons[button_idx].disabled = true;
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
    }
  }
}
