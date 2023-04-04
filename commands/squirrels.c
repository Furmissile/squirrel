struct discord_components* build_squirrel_buttons(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = SQUIRREL_SIZE;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  if (event->data->custom_id)
  {
    int button_idx = event->data->custom_id[1] -48;

    if (player.acorn_count >= squirrels[button_idx].acorn_count_req)
    {
      rewards.item_type = button_idx;
      player.squirrel = button_idx;
    }
    else
      rewards.item_type = ERROR_STATUS;
  }

  for (int i = 0; i < buttons->size; i++)
  {
    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    emoji->name = squirrels[i].squirrel.emoji_name;
    emoji->id = squirrels[i].squirrel.emoji_id;

    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      /*
        if no custom id && enough acorn count && this squirrel is not the selected squirrel
        OR
        custom id && enough acorn count && this squirrel is the selected squirrel
      */
      .style = ( (!event->data->custom_id && player.acorn_count >= squirrels[i].acorn_count_req && player.squirrel != i)
        || (event->data->custom_id && player.acorn_count >= squirrels[i].acorn_count_req && player.squirrel == i) ) 
          ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
      .custom_id = format_str(SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_SQUIRREL, i, event->member->user->id),
      .label = squirrels[i].squirrel.formal_name,
      .emoji = emoji,
      .disabled = (event->data->custom_id || player.acorn_count < squirrels[i].acorn_count_req
        || player.squirrel == i) ? true : false
    };
  }

  return buttons;
}

enum SQ_SHOP_FORMAT {
  SQ_SHOP_GENERAL,
  SQ_SHOP_SIZE
};

void squirrel_shop(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  discord_msg->buttons = build_squirrel_buttons(event);

  embed->title = format_str(SIZEOF_TITLE, "Squirrels");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      ""OFF_ARROW" Squirrels are unlocked based on your *current* acorn count "ACORN_COUNT"! \n"
      ""OFF_ARROW" Squirrels are lost upon health resetting.");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = SQ_SHOP_SIZE + SQUIRREL_SIZE;
  embed->fields->array = calloc(embed->fields->size, sizeof(struct discord_embed_field));

  embed->fields->array[SQ_SHOP_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[SQ_SHOP_GENERAL].value = format_str(SIZEOF_FIELD_VALUE, " "ACORN_COUNT" Acorn Count: **%s**", num_str(player.acorn_count) );

  for (int i = SQ_SHOP_SIZE; i < SQ_SHOP_SIZE + SQUIRREL_SIZE; i++)
  {
    int squirrel_idx = i - SQ_SHOP_SIZE;
    embed->fields->array[i].name = format_str(SIZEOF_TITLE, "<:%s:%ld> %s",
        squirrels[squirrel_idx].squirrel.emoji_name, squirrels[squirrel_idx].squirrel.emoji_id, squirrels[squirrel_idx].squirrel.formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE, " "OFF_ARROW" %s \n", 
        squirrels[squirrel_idx].squirrel.description);
    
    ADD_TO_BUFFER(embed->fields->array[i].value, SIZEOF_FIELD_VALUE,
        "*Requires* **%s** "ACORN_COUNT" acorn count",
        num_str( squirrels[squirrel_idx].acorn_count_req ) );
  }

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, squirrels[player.squirrel].squirrel.file_path);
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));

  if (event->data->custom_id)
  { // button index is rewards.item_type
    if (rewards.item_type == ERROR_STATUS)
    {
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You need a higher acorn count!");
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_NO_ACORNS].file_path);
    }
    else {
      struct sd_file_data *squirrel = &squirrels[rewards.item_type].squirrel;
      embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You are now the %s!", squirrel->formal_name);
      embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, squirrel->file_path);
    }
  } // error status can only be the case with a custom id
  else {
    embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Welcome to the Squirrel Shop!");
    embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }

}

int squirrels_interaction(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  player = load_player_struct(event->member->user->id);
  energy_regen();

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  squirrel_shop(event, discord_msg);

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
