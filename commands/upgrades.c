struct sd_store *upgrades;

struct discord_components* build_upgrade_buttons(
  const struct discord_interaction *event,
  int button_size)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  if (event->data->custom_id)
  {
    int current_stat = event->data->custom_id[1] - 48;
    int* stat_lv_ptr = upgrades[current_stat].stat_ptr;

    if (player.acorns >= upgrades[current_stat].cost)
    {
      player.acorns -= upgrades[current_stat].cost;
      rewards.item_type = current_stat;
      (*stat_lv_ptr)++;

      if (current_stat == STAT_STRENGTH)
        player.health += STRENGTH_VALUE;
    }
    else
      rewards.item_type = ERROR_STATUS;
  }

  for (int i = 0; i < buttons->size; i++)
  {
    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    int is_evolution = ((*upgrades[i].stat_ptr +1) % STAT_EVOLUTION == 0) ? 1 : 0;

    if (is_evolution) {
      emoji->name = evo_squirrels[player.squirrel].emoji_name;
      emoji->id = evo_squirrels[player.squirrel].emoji_id;
    } else {
      emoji->name = squirrels[player.squirrel].emoji_name;
      emoji->id = squirrels[player.squirrel].emoji_id;
    }

    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = (player.acorns >= upgrades[i].cost) 
                ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
      .custom_id = format_str(SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_UPGRADE, i, event->member->user->id),
      .label = upgrades[i].item->formal_name,
      .emoji = emoji
    };
  }

  return buttons;
}

enum STORE_FORMAT
{
  STORE_GENERAL = 0,
  STORE_SIZE = 1,
};

void player_shop(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  discord_msg->buttons = build_upgrade_buttons(event, STAT_SIZE);

  if (event->data->custom_id)
    for (int i = 0; i < STAT_SIZE; i++)
      upgrades[i] = (struct sd_store)
      {
        .item = &stats[i].stat,
        .cost = generate_price(*stats[i].stat_ptr, stats[i].price_mult),
        .stat_ptr = stats[i].stat_ptr
      };

  embed->title = format_str(SIZEOF_TITLE, "Upgrades");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = STORE_SIZE + STAT_SIZE;
  embed->fields->array = calloc(STORE_SIZE + STAT_SIZE, sizeof(struct discord_embed_field));

  /* Fill in player balance */
  embed->fields->array[STORE_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[STORE_GENERAL].value = format_str(SIZEOF_FIELD_VALUE, "> "ACORNS" Acorns: **%s** \n", num_str(player.acorns) );

  /* Fill in upgade information in separate fields */
  for (int i = STORE_SIZE; i < STORE_SIZE + STAT_SIZE; i++)
  {  
    int stat_idx = i - STORE_SIZE;
    //References
    char* stahr_type = (*stats[stat_idx].stat_ptr < BRONZE_BRACKET) ? BRONZE_STAHR
      : (*stats[stat_idx].stat_ptr < SILVER_BRACKET) ? SILVER_STAHR : STAHR;

    embed->fields->array[i].name = format_str(SIZEOF_TITLE, 
        "%s (%s **%d**)", 
        stats[stat_idx].stat.formal_name, stahr_type, *stats[stat_idx].stat_ptr);

    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        (stat_idx == STAT_STRENGTH) ? " "OFF_ARROW" %s (+**%0.0f**) \n" : " "OFF_ARROW" %s (x**%0.1f**) \n",
        stats[stat_idx].stat.description, 
        generate_factor(*stats[stat_idx].stat_ptr, stats[stat_idx].value_mult) );

    ADD_TO_BUFFER(embed->fields->array[i].value, SIZEOF_FIELD_VALUE,
        "*Costs* **%s** "ACORNS" Acorns",
        num_str( upgrades[stat_idx].cost ) );
  }

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, squirrels[player.squirrel].file_path);
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));

  if (event->data->custom_id)
  { // button index is rewards.item_type
    if (rewards.item_type == ERROR_STATUS)
    {
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You need more acorns!");
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else {
      struct sd_file_data *stat_index = &stats[rewards.item_type].stat;
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You received the stat of %s!", stat_index->formal_name);
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, stat_index->file_path);
    }
  } // error status can only be the case with a custom id
  else {
    embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Welcome to the Upgrades Shop!");
    embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

/* Listens for slash command interactions */
int store_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  player = load_player_struct(event->member->user->id);

  upgrades = calloc(STAT_SIZE, sizeof(struct sd_store));

  for (int i = 0; i < STAT_SIZE; i++)
    upgrades[i] = (struct sd_store)
    {
      .item = &stats[i].stat,
      .cost = generate_price(*stats[i].stat_ptr, stats[i].price_mult),
      .stat_ptr = stats[i].stat_ptr
    };

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  player_shop(event, discord_msg);

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