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

// welcome embed when a user joins the support server
void welcome_embed(struct discord *client, const struct discord_guild_member *member)
{
  if (member->guild_id != GUILD_ID) return;

  struct sd_header_params header = { 0 };

  struct sd_welcome_info params = { 0 };

  char embed_description[512] = { };

  header.embed = (struct discord_embed)
  {
    .color = (int)strtol("00eeff", NULL, 16),
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          member->user->id, member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Welcome to Squirrel Dash!"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrels[GRAY_SQUIRREL].squirrel.file_path)
    },
    .description = u_snprintf(embed_description, sizeof(embed_description),
        " "OFF_ARROW" Read the <#1046640388456321126> to <#1046813534790635550>! \n"
        " "OFF_ARROW" Begin your adventure in <#1046635264883294259>! \n"
        " "OFF_ARROW" Chat with fellow squirrel advocators in <#1046628380222685255> \n"
        " "OFF_ARROW" Looking for extra help? Ask away in <#1047233819201261748>!"),
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          misc[MISC_WELCOME].file_path)
    },
    .footer = &(struct discord_embed_footer) {
        .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
        .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, slice_types[TYPE_ACORNS].item.file_path)
      }
  };

  char msg_content[256] = { };

  struct discord_create_message intro_message = (struct discord_create_message) {
    .content = u_snprintf(msg_content, sizeof(msg_content), " "ACORNS" *<@%ld> is looking for acorns...* "ACORNS" ", member->user->id),
    .embeds = &(struct discord_embeds) {
      .array = &header.embed,
      .size = 1
    }
  };

  discord_create_message(client, WELCOME_CHANNEL_ID, &intro_message, NULL);
}

// mechanic for reaction verification
void verify(struct discord *client, const struct discord_message_reaction_add *event)
{
  if (event->member->user->id == APPLICATION_ID || event->channel_id != VERIFY_CHANNEL_ID)
    return;

  // delete whatever emoji user reacted with
  discord_delete_user_reaction(client, event->channel_id, event->message_id, event->member->user->id, 
      event->emoji->id, event->emoji->name, NULL);

  // only assign member role if they reacted with the acorn
  if (event->emoji->id != slice_types[TYPE_ACORNS].item.emoji_id)
    return;

  discord_add_guild_member_role(client, GUILD_ID, event->member->user->id, MEMBER_ROLE_ID, 
  &(struct discord_add_guild_member_role) { .reason = "New member" }, 
  NULL);
}