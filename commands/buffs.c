struct discord_components* build_buff_buttons(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = BUFFS_SIZE;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  int golden_acorn_cost = GOLDEN_ACORN_BUFF_COST * generate_factor(player.stats.luck_lv, LUCK_VALUE);

  if (event->data->custom_id)
  {
    if (player.golden_acorns >= golden_acorn_cost)
    {
      int button_idx = event->data->custom_id[1] -48;
      (*enchanted_acorns[button_idx].stat_ptr) += genrand(15, 5);
      rewards.item_type = button_idx;
      player.golden_acorns -= golden_acorn_cost;
    }
    else
      rewards.item_type = ERROR_STATUS;
  }

  int is_disabled = false;
  int button_style;
  if (player.golden_acorns >= golden_acorn_cost)
  {
    button_style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    button_style = DISCORD_BUTTON_SECONDARY;
    is_disabled = true;
  }

  for (int i = 0; i < buttons->size; i++)
  {
    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
    emoji->name = enchanted_acorns[i].item.emoji_name;
    emoji->id = enchanted_acorns[i].item.emoji_id;

    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = button_style,
      .custom_id = format_str(SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_E_ACORN, i, event->member->user->id),
      .label = enchanted_acorns[i].item.formal_name,
      .emoji = emoji,
      .disabled = is_disabled
    };

  }

  return buttons;
}

enum POWER_FORMAT {
  POWER_GENERAL = 0,
  POWER_PRICES = 1,
  POWER_SIZE = 2
};

void power_shop(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  discord_msg->buttons = build_buff_buttons(event);

  embed->title = format_str(SIZEOF_TITLE, "Buffs Shop");

  embed->description = format_str(SIZEOF_DESCRIPTION,
    ""OFF_ARROW" Buffs are stackable and are only used when applied. \n"
    ""OFF_ARROW" Active buffs show up with a "QUEST_MARKER" and its duration. \n");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = POWER_SIZE + BUFFS_SIZE;
  embed->fields->array = calloc(POWER_SIZE + BUFFS_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[POWER_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[POWER_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      "\n > "GOLDEN_ACORNS" Golden Acorns: **%s**",
      num_str(player.golden_acorns) );

  embed->fields->array[POWER_PRICES].name = format_str(SIZEOF_TITLE, "Enchanted Acorn Cost");
  embed->fields->array[POWER_PRICES].value = format_str(SIZEOF_FIELD_VALUE, 
      "> **%s** "GOLDEN_ACORNS" Golden Acorns \n", 
      num_str(GOLDEN_ACORN_BUFF_COST * generate_factor(player.stats.luck_lv, LUCK_VALUE)) );

  for (int i = POWER_SIZE; i < POWER_SIZE + BUFFS_SIZE; i++)
  {
    int buffs_idx = i - POWER_SIZE;
  
    if (*enchanted_acorns[buffs_idx].stat_ptr > 0)
      embed->fields->array[i].name = format_str(SIZEOF_TITLE,
          "<:%s:%ld> %s ("QUEST_MARKER" **%s**) \n", 
          enchanted_acorns[buffs_idx].item.emoji_name, enchanted_acorns[buffs_idx].item.emoji_id, 
          enchanted_acorns[buffs_idx].item.formal_name, num_str(*enchanted_acorns[buffs_idx].stat_ptr) );
    else
      embed->fields->array[i].name = format_str(SIZEOF_TITLE,
          "<:%s:%ld> %s \n", 
          enchanted_acorns[buffs_idx].item.emoji_name, enchanted_acorns[buffs_idx].item.emoji_id, 
          enchanted_acorns[buffs_idx].item.formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        " "OFF_ARROW" %s \n", enchanted_acorns[buffs_idx].item.description);
  }

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));

  if (event->data->custom_id)
  { // button index is rewards.item_type
    if (rewards.item_type == ERROR_STATUS)
    {
      embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, SQ_CHEM_PATH);
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You need more acorn!");
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else {
      embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, enchanted_acorns[rewards.item_type].item.file_path);
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You received the %s!", enchanted_acorns[rewards.item_type].item.formal_name);
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, enchanted_acorns[rewards.item_type].item.file_path);
    }
  }
  else {
    embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, SQ_CHEM_PATH);
    embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "/help | Details on how enchanted acorns work!");
    embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
  }

}

/* Listens for slash command interactions */
int buffs_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  power_shop(event, discord_msg);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = discord_msg->buttons
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg->buttons);
  free(discord_msg);

  update_player_row(player);
  rewards = (struct sd_rewards) { 0 };

  return 0;
}