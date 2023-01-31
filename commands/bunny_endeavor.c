struct sd_store *bunny_store;

struct discord_components* build_bunny_buttons(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = BUNNY_STORE_SIZE;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  if (event->data->custom_id)
  {
    int button_idx = event->data->custom_id[1] -48;
    int* stat_ptr = bunny_store[button_idx].stat_ptr;

    if (player.events.catnip >= bunny_store[button_idx].cost)
    {
      player.events.catnip -= bunny_store[button_idx].cost;
      rewards.item_type = button_idx;
      (*stat_ptr) += bunny_store[button_idx].quantity;
    }
    else
      rewards.item_type = ERROR_STATUS;
  }

  for (int i = 0; i < buttons->size; i++)
  {
    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    emoji->name = bunny_store[i].item->emoji_name;
    emoji->id = bunny_store[i].item->emoji_id;

    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = (player.events.catnip >= bunny_store[i].cost) 
          ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
      .custom_id = format_str(SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_E_ACORN, i, event->member->user->id),
      .label = bunny_store[i].item->formal_name,
      .emoji = emoji
    };
  }

  return buttons;
}

enum BUNNY_FORMAT
{
  BUNNY_GENERAL,
  BUNNY_SIZE
};

void bunny_shop(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  discord_msg->buttons = build_bunny_buttons(event);

  embed->title = format_str(SIZEOF_TITLE, "Bunny's Wares");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      ""OFF_ARROW" Purchase a resource with Catnip ("CATNIP")! \n"
      ""OFF_ARROW" Acorn and golden acorns increase with your stats. \n");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = STORE_SIZE + BUNNY_STORE_SIZE;
  embed->fields->array = calloc(embed->fields->size, sizeof(struct discord_embed_field));

  /* Fill in player balance */
  embed->fields->array[BUNNY_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[BUNNY_GENERAL].value = format_str(SIZEOF_FIELD_VALUE, 
      "> "CATNIP" Catnip: **%s** \n"
      "> "ACORNS" Acorns: **%s** \n"
      "> "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
      "> "ENERGY" Energy: **%s**/%d \n", 
      num_str(player.events.catnip), num_str(player.acorns), num_str(player.golden_acorns),
      num_str(player.energy), MAX_ENERGY );

  /* Fill in upgade information in separate fields */
  for (int i = BUNNY_SIZE; i < BUNNY_SIZE +BUNNY_STORE_SIZE; i++)
  {
    int item_idx = i - BUNNY_SIZE;

    struct sd_file_data *item_index = bunny_store[item_idx].item;
    
    embed->fields->array[i].name = format_str(SIZEOF_TITLE, "%s <:%s:%ld> %s", 
        num_str(bunny_store[item_idx].quantity), 
        item_index->emoji_name, item_index->emoji_id, item_index->formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        " "INDENT" *Requires* **%s** "CATNIP" Catnip \n",
        num_str(bunny_store[item_idx].cost) );
  }

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, BUNNY_ED_PATH);

  embed->footer = calloc(1, sizeof(struct discord_embed_footer));

  if (event->data->custom_id)
  { // button index is rewards.item_type
    if (rewards.item_type == ERROR_STATUS)
    {
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You need more acorns!");
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else
    {
      struct sd_file_data *item_index = bunny_store[rewards.item_type].item;
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You received %d %s!", 
          bunny_store[rewards.item_type].quantity, item_index->formal_name);
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_index->file_path);
    }
  } // error status can only be the case with a custom id
  else {
    embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Welcome to the Bunny's Wares!");
    embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }

}


int bunny_interaction(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct tm *info = get_UTC();
  ERROR_INTERACTION((info->tm_mday < 21), "This event is not active!");

  player = load_player_struct(event->member->user->id);
  energy_regen();

  bunny_store = (struct sd_store[]) 
  {
    {
      .item = &items[ITEM_ACORNS].item,
      .cost = 500,
      .quantity = UNIT_ACORN * (*stats[STAT_PROFICIENCY].stat_ptr +1),
      .stat_ptr = &player.acorns
    },
    {
      .item = &items[ITEM_GOLDEN_ACORN].item,
      .cost = 500,
      .quantity = UNIT_ACORN/2 * (*stats[STAT_LUCK].stat_ptr +1),
      .stat_ptr = &player.golden_acorns
    },
    {
      .item = &items[ITEM_ENERGY].item,
      .cost = 750,
      .quantity = 50,
      .stat_ptr = &player.energy
    }
  };

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  bunny_shop(event, discord_msg);

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
