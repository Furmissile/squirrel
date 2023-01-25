struct sd_store *upgrades;

/* Handles upgrading a stat */
struct discord_component upgrade_stat(
  const struct discord_interaction *event, 
  struct discord_component current_button,
  int current_stat)
{
  int* stat_lv_ptr = upgrades[current_stat].stat_ptr;
  int stat_cost = upgrades[current_stat].cost;

  //if there's a custom id, this is a response
  if (event->data->custom_id
    && event->data->custom_id[1] -48 == current_stat // compares button index at [1] to button index of button pressed
    && player.acorns >= stat_cost )
  {
    player.acorns -= stat_cost;

    (*stat_lv_ptr)++;

    if (current_stat == STAT_STRENGTH)
      player.health += STRENGTH_VALUE;
  }

  //build the button regardless to account for updated price
  if (player.acorns >= stat_cost)
  {
    current_button.style = DISCORD_BUTTON_PRIMARY;
  } 
  else {
    current_button.style = DISCORD_BUTTON_SECONDARY;
    current_button.disabled = true;
  }

  return current_button;
}

struct discord_components* build_upgrade_buttons(
  const struct discord_interaction *event,
  int button_size)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++)
  {
    buttons->array[i] = upgrade_stat(event, buttons->array[i], i);

    int is_evolution = ((*upgrades[i].stat_ptr +1) % STAT_EVOLUTION == 0) ? 1 : 0;

    buttons->array[i].label = upgrades[i].item->formal_name;

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    if (is_evolution) {
      emoji->name = evo_squirrels[player.squirrel].emoji_name;
      emoji->id = evo_squirrels[player.squirrel].emoji_id;
    } else {
      emoji->name = squirrels[player.squirrel].emoji_name;
      emoji->id = squirrels[player.squirrel].emoji_id;
    }

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_UPGRADE, i, event->member->user->id);
    // i is used instead of a char!!! Match i with biome type to get squirrel!

    buttons->array[i].custom_id = set_custom_id;
    buttons->array[i].type = DISCORD_COMPONENT_BUTTON;
    buttons->array[i].emoji = emoji;
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

  return 0;
}