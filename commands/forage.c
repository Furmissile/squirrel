/* Handle rewards based on item_type for main embed*/
void get_rewards(char msg_id) 
{
  switch (rewards.item_type) 
  {
    case TYPE_ACORN_HANDFUL:
      rewards.acorns = genrand(25, 15);
      break;
    case TYPE_ACORN_MOUTHFUL:
      rewards.acorns = genrand(50, 25);
      if (msg_id == TYPE_ENCOUNTER_MSG)
        rewards.golden_acorns = genrand(25, 25);
      break;
    case TYPE_LOST_STASH:
      rewards.acorns = genrand(75, 25);
      rewards.golden_acorns = genrand(25, 25);
      break;
    case TYPE_HEALTH_LOSS:
      rewards.health_loss = genrand(10, 5) + (player.biome_num *5);
      break;
    case TYPE_ACORN_SACK:
      rewards.acorns = genrand(100, 25);
      if (msg_id == TYPE_ENCOUNTER_MSG)
        rewards.golden_acorns = genrand(25, 25);
  }

  // TODO: Factor health regen stat
  // 5 is the max with base stat
  if (player.health <= player.max_health - BASE_HEALTH_REGEN
    && rewards.item_type != TYPE_HEALTH_LOSS
    && rand() % MAX_CHANCE > 80)
  {
    rewards.health_regen = BASE_HEALTH_REGEN;
    player.health += rewards.health_regen;
  }
  
  if (rewards.acorns)
  {
    // base amount goes to war stash and acorn count
    if (scurry.war_flag == 1 && (rand() % MAX_CHANCE) > 65)
      factor_war();
    else if (scurry.war_flag == 0)
      scurry.war_acorns = (scurry.war_acorns + rewards.acorns >= scurry.war_acorn_cap) 
          ? scurry.war_acorn_cap : scurry.war_acorns + rewards.acorns;

    player.acorn_count += rewards.acorns;

    // apply stat and buff
    rewards.acorns *= generate_factor(player.stats.proficiency_lv, PROFICIENCY_VALUE);
    if (player.buffs.proficiency_acorn > 0)
    {
      rewards.acorns *= 2;
      player.buffs.proficiency_acorn--;
    }

    // passive scurry buff
    rewards.acorns *= (scurry.rank > SEED_NOT) ? ((BASE_COURAGE_MULT * (scurry.rank +1)) +1) : 1;

    // only acorns gets multiplied -- the rest are added making the func call here no different
    factor_season();

    player.acorns += rewards.acorns;
  }

  if (rewards.golden_acorns)
  { // apply stat then buff
    rewards.golden_acorns *= generate_factor(player.stats.luck_lv, LUCK_VALUE);
    if (player.buffs.luck_acorn > 0)
    {
      rewards.golden_acorns *= 2;
      player.buffs.luck_acorn--;
    }

    player.golden_acorns += rewards.golden_acorns;
  }

  if (rewards.health_loss)
  {
    if (player.buffs.defense_acorn > 0)
    {
      rewards.health_loss /= 2;
      player.buffs.defense_acorn--;
    }
    if (player.health - rewards.health_loss < 0)
      player.health = 0;
    else
      player.health -= rewards.health_loss;
    
    if (player.health == 0)
      player.golden_acorns += (player.biome +1) * DIVIDEND_VALUE * player.acorn_count;
  }

  player.events.catnip += rewards.catnip;
}

/* Load rewards onto main embed */
void generate_rewards(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  get_rewards(event->data->custom_id[0]);

  if (rewards.acorns)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n+**%s** "ACORNS" Acorns \n%s", 
      num_str(rewards.acorns), 
      (player.buffs.proficiency_acorn > 0) ? "-**1** "PROFICIENCY_ACORN" Acorn of Proficiency \n" : " " );
  else
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\nYou received no earnings! \n");

  if (rewards.golden_acorns)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n+**%s** "GOLDEN_ACORNS" Golden Acorns \n%s", 
      num_str(rewards.golden_acorns), 
      (player.buffs.luck_acorn > 0) ? "-**1** "LUCK_ACORN" Acorn of Luck \n" : " " );

  if (rewards.catnip)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n+**%s** "CATNIP" Catnip \n", num_str(rewards.catnip));
  
  if (rewards.health_loss)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n-**%s** "BROKEN_HEALTH" HP (**%s** "HEALTH" HP Left) \n%s", 
      num_str(rewards.health_loss), 
      num_str(player.health), 
      (player.buffs.defense_acorn > 0) ? "-**1** "DEFENSE_ACORN" Acorn of Defense \n" : " " );
  
  if (rewards.health_regen)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n+**%s** "HEALTH" HP (**%s** "HEALTH" HP Left) \n", 
      num_str(rewards.health_regen),
      num_str(player.health) );

  if (player.health == 0)
  {
    player.health = MAX_HEALTH + generate_factor(player.stats.strength_lv, STRENGTH_VALUE);

    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n No more health! \n"
        ""QUEST_MARKER" Your highest acorn count was **%s** "ACORNS" Acorns! \n"
        "+**%s** "GOLDEN_ACORNS" Golden Acorns \n"
        "Your score was reset. \n", 
        num_str(player.high_acorn_count), num_str((player.biome +1) * DIVIDEND_VALUE * player.acorn_count) );
    // final acorns are added and then high score is set
    // high score is overwritten regardless of acorn count being higher
    player.high_acorn_count = player.acorn_count;
    player.acorn_count = 0;
  }

  if (rewards.stolen_acorns)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\nYou successfully stole **%s** "STOLEN_ACORNS" War Acorns! \n+**%s** "COURAGE" Courage \n", 
        num_str(rewards.stolen_acorns), num_str(rewards.courage) );

  energy_status(discord_msg, MAIN_ENERGY_COST);
}

/* Build components after button press */
struct discord_components* main_button_response(
  const struct discord_interaction *event, 
  int msg_type) 
{
  struct discord_components* buttons = calloc(1, sizeof(struct discord_components));
  struct sd_encounter encounter = biomes[player.biome].encounters[player.encounter];

  buttons->size = 3;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++) 
  {
    int chance = rand() % MAX_CHANCE;

    int button_item = 0;

    if (msg_type == TYPE_MAIN_MSG)
      button_item = 
          (chance < JUNK_CHANCE) ? TYPE_NO_ACORNS
        : (chance < COMMON_CHANCE) ? TYPE_ACORN_HANDFUL
        : (chance < UNCOMMON_CHANCE) ? TYPE_ACORN_MOUTHFUL
        : (chance < CONTAINER_CHANCE) ? TYPE_LOST_STASH : TYPE_ACORN_SACK;
    else
      button_item =
          (chance < NORMAL_CHANCE) ? TYPE_ACORN_MOUTHFUL
        : (chance < HEALTH_LOSS_CHANCE) ? TYPE_HEALTH_LOSS : TYPE_ACORN_SACK;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", msg_type, i, event->member->user->id);

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
    emoji->name = item_types[button_item].file_path;
    emoji->id = item_types[button_item].emoji_id;
    
    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = emoji,
      .custom_id = set_custom_id,
      .label = (msg_type == TYPE_ENCOUNTER_MSG) ? encounter.solutions[i] : NULL,
      .disabled = true
    };

    if (event->data->custom_id[1] == set_custom_id[1])
    {
      rewards.item_type = button_item;
      buttons->array[i].style = DISCORD_BUTTON_PRIMARY;
    }
    else 
      buttons->array[i].style = DISCORD_BUTTON_SECONDARY;
  }

  return buttons;
}

/* Build components on embed startup */
struct discord_components* build_buttons(
  const struct discord_interaction *event,
  int emoji_type,
  int msg_type) 
{
  struct discord_components* buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = 3;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
  emoji->name = item_types[emoji_type].emoji_name;
  emoji->id = item_types[emoji_type].emoji_id;

  struct sd_encounter encounter = biomes[player.biome].encounters[player.encounter];

  for (int i = 0; i < buttons->size; i++) {
    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    // 3rd letter to distinguish between encounters and normal messsages
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", msg_type, i, event->member->user->id);

    buttons->array[i] = (struct discord_component) {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_PRIMARY,
      .custom_id = set_custom_id,
      .emoji = emoji,
      .label = (msg_type == TYPE_ENCOUNTER_MSG) ? encounter.solutions[i] : NULL
    };
  }

  return buttons;
}

/* If an encounter was found, generate embed or response depending on response_flag */
void encounter_embed(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  struct sd_encounter encounter = biomes[player.biome].encounters[player.encounter];

  embed->title = format_str(SIZEOF_TITLE, encounter.name);

  embed->description = format_str(SIZEOF_DESCRIPTION, "*%s*\n", encounter.conflict);

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, encounter.file_path);

  if (event->data->custom_id[0] == TYPE_ENCOUNTER_MSG) 
  { //this is an encounter response
    discord_msg->buttons = main_button_response(event, TYPE_ENCOUNTER_MSG);

    generate_rewards(event, discord_msg);
    player.main_cd = time(NULL) + COOLDOWN;
  }
  else {
    discord_msg->buttons = build_buttons(event, TYPE_ENCOUNTER, TYPE_ENCOUNTER_MSG);
  }

}

void main_embed(
  const struct discord_interaction *event,
  struct sd_message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));

  if (event->data->custom_id)
  { // this is a response
    if (event->data->custom_id[0] == TYPE_ENCOUNTER_MSG) 
    { // this is an encounter response
      encounter_embed(event, discord_msg);
    } 
    else if ((rand() % MAX_CHANCE) < ENCOUNTER_CHANCE && scurry.war_flag == 0)
    { // otherwise, can the player get an encounter? (scurry cannot be at war)
      player.encounter = rand() % biomes[player.biome].encounter_size;
      encounter_embed(event, discord_msg);
    }
    else { // if not, create the normal response
      discord_msg->buttons = main_button_response(event, TYPE_MAIN_MSG);

      embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
      embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, item_types[rewards.item_type].file_path);

      embed->title = format_str(SIZEOF_TITLE, "You found %s!", item_types[rewards.item_type].formal_name);

      generate_rewards(event, discord_msg);

      player.main_cd = time(NULL) + COOLDOWN;
    } 
  } else { // this is an init
    embed->title = format_str(SIZEOF_TITLE, "Scouring the lands...");

    discord_msg->buttons = build_buttons(event, TYPE_DIRT_PILE, TYPE_MAIN_MSG);
  }

  embed->image = calloc(1, sizeof(struct discord_embed_image) );
  embed->image->url = format_str(SIZEOF_URL, GIT_PATH, biomes[player.biome].biome_scene_path);
}

int forage_interaction(const struct discord_interaction *event, struct sd_message *discord_msg)
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  energy_regen();

  ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready!");
  ERROR_INTERACTION((player.energy < 2), "You need more energy!");

  main_embed(event, discord_msg);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

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
  update_scurry_row(scurry);
  rewards = (struct sd_rewards) { 0 };

  return 0;
}