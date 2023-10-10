
/* Returns total factor based on stat level and if it's a multiplier or incrementer */
float generate_factor(int stat_lv, double value_factor)
{
  if (value_factor == STRENGTH_FACTOR)
    return (value_factor * (stat_lv -1)) + (5 * (stat_lv/STAT_EVOLUTION));
  else
    return (value_factor * (stat_lv -1)) + (value_factor * (stat_lv/STAT_EVOLUTION)) +1;
}

/* Returns a total price based on stat level, currency unit, and a value multiplier */
int generate_price(int stat_lv, double value_mult)
{
  return ((stat_lv +1)/STAT_EVOLUTION * value_mult + value_mult) * stat_lv;
}

int energy_status(char* sd_description, size_t description_size, struct sd_player *player, int energy_loss)
{
  int energy_chance = 100; // default energy chance
  int is_boosted = 0; // if boosted acorn can be used

  // compute chance
  if (player->squirrel == GRAY_SQUIRREL)
  {
    energy_chance = 80;
    if (player->buffs.boosted_acorn > 0) // boost is only applied if squirrel is active!
    {
      energy_chance = 60;
      is_boosted = 1;
    }
  }

  if (rand() % MAX_CHANCE > energy_chance
    && energy_loss == MAIN_ENERGY_COST) // only for foraging
  {
    if (is_boosted)
    {
      player->buffs.boosted_acorn--;
      u_snprintf(sd_description, description_size, "\n-**10**%% "BOOSTED_ACORN" Squirrel Charge (**%d**%% Left)", player->buffs.boosted_acorn *10);
    }
    return 0;
  }
  else {
    player->energy -= energy_loss;
    u_snprintf(sd_description, description_size, "\n-**%d** "ENERGY" Energy \n", energy_loss);
  }

  return energy_loss;
}

void energy_regen(struct sd_player *player) 
{
  if (APPLICATION_ID == BETA_BOT_ID && player->energy < 2)
    player->energy = MAX_ENERGY;

  struct tm *info = get_UTC();

  int energy_cd = (info->tm_mday > 14 && info->tm_mday < 21) ? 90 : 180;

  int add_energy = (player->energy < MAX_ENERGY) ? (time(NULL) - player->main_cd)/ energy_cd : 0;

  if (player->energy + add_energy > MAX_ENERGY)
    player->energy += (player->energy > MAX_ENERGY) ? 0 : add_energy - (add_energy - (MAX_ENERGY - player->energy));
  else
    player->energy += add_energy;

  // wont disturb cooldown when info embed is sent
  player->main_cd = time(NULL) - ((time(NULL) - player->main_cd) % energy_cd);
}

int factor_season(struct sd_player *player, struct sd_rewards *rewards)
{
  struct tm *info = get_UTC();

  if (info->tm_mday < 7) 
  {
    if (rand() % MAX_CHANCE > 80)
    {
      rewards->seasoned_golden_acorns = (rewards->item_type < TYPE_ACORN_MOUTHFUL) ? genrand(25, 10)
          : (rewards->item_type < TYPE_LOST_STASH) ? genrand(50, 25) : genrand(100, 50);

      rewards->seasoned_golden_acorns += (BIOME_GOLDEN_ACORN_INC * (player->biome_num + player->biome_num / BIOME_SIZE));
      rewards->seasoned_golden_acorns *= generate_factor(player->stats.luck_lv, LUCK_FACTOR);
      
      player->golden_acorns += rewards->seasoned_golden_acorns;
      player->session_data.golden_acorns += rewards->seasoned_golden_acorns;
    }
    return SPRING_MULT;
  }
  else if (info->tm_mday < 14) 
  {
    if (rand() % MAX_CHANCE > VICTUALS_CHANCE)
    {
      // resource overflow is acceptable in this case since the chance is so low
      int victuals_chance = rand() % MAX_CHANCE;

      if (victuals_chance < BLUEBERRY_CHANCE) 
      {
        rewards->victual_amt = (rewards->item_type < TYPE_ACORN_MOUTHFUL) ? genrand(75, 25)
            : (rewards->item_type < TYPE_LOST_STASH) ? genrand(125, 25) : genrand(150, 50);
        
        rewards->victual_type = VICTUALS_BLUEBERRY;
        rewards->ref_resource = ITEM_ACORN_COUNT;
        player->acorn_count += rewards->victual_amt;
        player->session_data.acorn_count += rewards->victual_amt;
      }
      else if (victuals_chance < CHERRY_CHANCE && player->health < player->max_health) 
      {
        rewards->victual_amt = genrand(BASE_HEALTH_REGEN, BASE_HEALTH_REGEN) * player->stats.strength_lv;
        rewards->victual_type = VICTUALS_CHERRY;
        rewards->ref_resource = ITEM_HEALTH;
        player->health += rewards->victual_amt;
      }
      else {
        rewards->victual_amt = (rewards->item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
            : (rewards->item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);
        rewards->victual_type = VICTUALS_SEED;
        rewards->ref_resource = ITEM_ENERGY;
        player->energy += rewards->victual_amt;
      }
    }
    return SUMMER_MULT;
  }
  else if (info->tm_mday < 21)
    return FALL_MULT;

  if (rand() % MAX_CHANCE > 80)
  {
    rewards->catnip = (rewards->item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
        : (rewards->item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);

    player->catnip += rewards->catnip;
  }
  return WINTER_MULT;
}

// retain priority order
void print_rewards(char* sd_description, size_t description_size, struct sd_player *player, struct sd_rewards *rewards, struct sd_buff_status *buff_status)
{
  // ACORNS
  APPLY_NUM_STR(acorns, rewards->acorns);
  APPLY_NUM_STR(acorn_count, rewards->acorn_count);

  u_snprintf(sd_description, description_size, "\n+**%s** "ACORNS" Acorns \n", acorns);

  if (player->squirrel == SQUIRREL_BOOKIE && buff_status->boosted_acorn)
    u_snprintf(sd_description, description_size, "-**10**%% "BOOSTED_ACORN" Squirrel Charge (**%d**%% Left) \n", player->buffs.boosted_acorn *10);

  u_snprintf(sd_description, description_size, "\n+**%s** "ACORN_COUNT" Acorn Count \n", acorn_count);
  if (buff_status->proficiency_acorn)
    u_snprintf(sd_description, description_size, "-**1** "PROFICIENCY_ACORN" Acorn of Proficiency (**%d** left) \n", player->buffs.proficiency_acorn);
  if (player->squirrel == KING_SQUIRREL && buff_status->boosted_acorn)
    u_snprintf(sd_description, description_size, "-**10**%% "BOOSTED_ACORN" Squirrel Charge (**%d**%% Left) \n", player->buffs.boosted_acorn *10);

  // GOLDEN ACORNS (if any)
  if (rewards->golden_acorns) 
  {
    APPLY_NUM_STR(reward, rewards->golden_acorns);

    u_snprintf(sd_description, description_size, "\n+**%s** "GOLDEN_ACORNS" Golden Acorns \n", reward);
        
    if (buff_status->luck_acorn)
      u_snprintf(sd_description, description_size, "-**1** "LUCK_ACORN" Acorn of Luck (**%d** left) \n", player->buffs.luck_acorn);

    if (player->squirrel == ANGELIC_SQUIRREL && buff_status->boosted_acorn)
      u_snprintf(sd_description, description_size,
          "-**10**%% "BOOSTED_ACORN" Squirrel Charge (**%d**%% left) \n",
          player->buffs.boosted_acorn *10);
  }

  if (buff_status->received_proficiency_buff)
      u_snprintf(sd_description, description_size, "\n "QUEST_MARKER" You received **%d** "PROFICIENCY_ACORN" *Acorns of Proficiency*! \n",
          player->buffs.proficiency_acorn);
  if (buff_status->received_luck_buff)
      u_snprintf(sd_description, description_size, "\n "QUEST_MARKER" You received **%d** "LUCK_ACORN" *Acorns of Luck*! \n",
          player->buffs.luck_acorn);

  // EVENT REWARDS (only one can occur)
  if (rewards->catnip)
  {
    u_snprintf(sd_description, description_size, "\n+**%d** "CATNIP" Catnip \n", rewards->catnip);
  }
  else if (rewards->victual_amt)
  {
    struct sd_file_data victual_data = victuals[rewards->victual_type];
    struct sd_file_data item_ref = items[rewards->ref_resource];
    
    APPLY_NUM_STR(victuals_amt, rewards->victual_amt);
    u_snprintf(sd_description, description_size, "\n+**%s** <:%s:%ld> %s (+**%s** <:%s:%ld> %s) \n",
        victuals_amt, victual_data.emoji_name, victual_data.emoji_id, victual_data.formal_name,
        victuals_amt, item_ref.emoji_name, item_ref.emoji_id, item_ref.formal_name);
  }
  else if (rewards->seasoned_golden_acorns)
  {
    u_snprintf(sd_description, description_size, "\n+**%d** "LUCK_ICON" Seasonal Golden Acorns \n", rewards->seasoned_golden_acorns);
  }

  // conjured acorns OR war acorns can be true
  if (rewards->conjured_acorns)
    u_snprintf(sd_description, description_size, "\n+**%d**%% "CONJURED_ACORNS" Squirrel Charge \n", 
        rewards->conjured_acorns *10, player->conjured_acorns *10);

  if (rewards->charge_ready)
    u_snprintf(sd_description, description_size, " "QUEST_MARKER" Squirrel Charge is ready! \n");

  // if there's war acorns, there's courage
  else if (rewards->stolen_acorns)
  {
    APPLY_NUM_STR(stolen_acorns, rewards->stolen_acorns);
    u_snprintf(sd_description, description_size, "\n+**%s** "WAR_ACORNS" Stolen Acorns \n", stolen_acorns);
  }
  else if (rewards->war_acorns)
    u_snprintf(sd_description, description_size, "\n+**%d** "WAR_ACORNS" War Acorns \n", rewards->war_acorns);

  if (player->spent_golden_acorns)
  {
    APPLY_NUM_STR(golden_acorns, player->golden_acorns);
    APPLY_NUM_STR(spent_golden_acorns, player->spent_golden_acorns);

    u_snprintf(sd_description, description_size,
        "\n-**%s** "GOLDEN_ACORNS" Golden Acorns (**%s** "GOLDEN_ACORNS" Golden Acorns Left) \n", 
        spent_golden_acorns, golden_acorns );
    
    player->spent_golden_acorns = 0;
  }

  // acorn count is updated before biome_num and can be used to predict when the biome will change at this stage
  if (player->acorn_count/BIOME_INTERVAL > player->biome_num)
  {
    struct sd_file_data next_biome_data = biomes[(player->biome_num +1) % BIOME_SIZE].biome_icon;
    u_snprintf(sd_description, description_size, "\n "QUEST_MARKER" You have reached <:%s:%ld> **%s**!\n",
        next_biome_data.emoji_name, next_biome_data.emoji_id, next_biome_data.formal_name);
  }
}

// base rewards includes acorns and golden acorns (if any)
void apply_base_rewards(struct sd_player *player, struct sd_rewards *rewards, struct sd_buff_status *buff_status)
{
  // give a bonus BIOME_ACORN_INC after completing a cycle!
  rewards->acorns += (BIOME_ACORN_INC * (player->biome_num + player->biome_num / BIOME_SIZE));

  // CURRENT ORDER: Buff -> Squirrel -> Stat
  
  // if buff is active
  if (player->buffs.proficiency_acorn)
  {
    rewards->acorns *= BUFF_CONSTANT;
    rewards->acorn_count *= BUFF_CONSTANT;
    player->buffs.proficiency_acorn--;
    buff_status->proficiency_acorn = true;
  }

  if (player->squirrel == SQUIRREL_BOOKIE)
  {
    rewards->acorns *= SQUIRREL_CONSTANT;
    // boost squirrel
    if (player->buffs.boosted_acorn > 0)
    {
      rewards->acorns *= BOOSTED_ACORN_CONSTANT;
      player->buffs.boosted_acorn--;
      buff_status->boosted_acorn = true;
    }
  }

  // stat is not included in acorn count
  rewards->acorns *= generate_factor(player->stats.proficiency_lv, PROFICIENCY_FACTOR);

  if (player->squirrel == KING_SQUIRREL)
  {
    rewards->acorn_count *= SQUIRREL_CONSTANT;
    // boost squirrel
    if (player->buffs.boosted_acorn > 0) 
    {
      rewards->acorn_count *= BOOSTED_ACORN_CONSTANT;
      player->buffs.boosted_acorn--;
      buff_status->boosted_acorn = true;
    }
  }

  player->acorn_count += rewards->acorn_count;
  player->session_data.acorn_count += rewards->acorn_count;

  // season bonus always applies to acorns -- golden acorns are handled separately
  // season bonuses must always be displayed last on rewards
  if (rewards->item_type != TYPE_NO_ACORNS && rewards->encounter_cost == 0) // no encounter cost implies not an encounter!
  {
    int season_factor = factor_season(player, rewards);
    rewards->acorns *= season_factor;
  }

  player->acorns += rewards->acorns;
  player->session_data.acorns += rewards->acorns;

  // GOLDEN ACORNS
  if (rewards->golden_acorns)
  {
    // give a bonus BIOME_GOLDEN_ACORN_INC after completing a cycle!
    rewards->golden_acorns += (BIOME_GOLDEN_ACORN_INC * (player->biome_num + player->biome_num / BIOME_SIZE));

    if (player->buffs.luck_acorn > 0) 
    {
      rewards->golden_acorns *= BUFF_CONSTANT;
      player->buffs.luck_acorn--;
      buff_status->luck_acorn = true;
    }

    // only source of golden acorns is from lost stashes!
    if (player->squirrel == ANGELIC_SQUIRREL && player->buffs.boosted_acorn > 0)
    {
      player->buffs.boosted_acorn--;
      buff_status->boosted_acorn = true;
    }

    rewards->golden_acorns *= generate_factor(player->stats.luck_lv + player->stats.strength_lv, LUCK_FACTOR);

    player->golden_acorns += rewards->golden_acorns;
    player->session_data.golden_acorns += rewards->golden_acorns;
  }

}

int get_season_event(const struct discord_interaction *event)
{
  struct tm *info = get_UTC();

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char* seasons[4] = {"Spring (Spring Chicken Live!)", "Summer (Garden Raid Season!)", "Fall (Hibernation Live!)", "Winter (Bunny's Endeavor Live!)"};

  char event_season[128] = { };

  char* season = seasons[(info->tm_mday/7 < 4) ? info->tm_mday/7 : 3];
  char* month = months[(info->tm_mday < 21) ? info->tm_mon : (info->tm_mon +1 == 12) ? 0 : info->tm_mon +1];
  int end_day = (info->tm_mday < 7) ? 7 : (info->tm_mday < 14) ? 14 : (info->tm_mday < 21) ? 21 : 1;

  // end_day can only be 1, otherwise it's greater than 3
  char* suffix = (end_day == 1) ? "st" : "th";

  u_snprintf(event_season, sizeof(event_season), "Current Season: **%s** (Ends on %s **%d%s**)", 
      season, month, end_day, suffix);

  discord_create_interaction_response(client, event->id, event->token, 
  &(struct discord_interaction_response)
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = event_season
    }
  },
  NULL);

  return 0;
}

void init_help_buttons(const struct discord_interaction *event, struct sd_help_info *params, int page_idx, char help_type, int last_topic)
{
  params->buttons[0] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏮️",
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]), 
        "%c0a_%ld", help_type, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->buttons[1] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏪",
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]), 
        "%c%db_%ld", help_type, (page_idx > 0) ? page_idx -1 : 0, event->member->user->id),
    .style = (page_idx > 0) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == 0) ? true : false
  };

  params->buttons[2] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏩",
    .custom_id = u_snprintf(params->custom_ids[2], sizeof(params->custom_ids[2]), 
        "%c%dc_%ld", help_type, (page_idx < last_topic) ? page_idx +1 : last_topic, event->member->user->id),
    .style = (page_idx < last_topic) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == last_topic) ? true : false
  };

  params->buttons[3] = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = "⏭️",
    .custom_id = u_snprintf(params->custom_ids[3], sizeof(params->custom_ids[3]),
        "%c%dd_%ld", help_type, last_topic, event->member->user->id),
    .style = (page_idx < last_topic) ? DISCORD_BUTTON_PRIMARY : DISCORD_BUTTON_SECONDARY,
    .disabled = (page_idx == last_topic) ? true : false
  };
}

void generate_util_buttons(const struct discord_interaction *event, struct sd_player *player, struct sd_util_info *params)
{
  params->emojis[0] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[0], sizeof(params->emoji_names[0]), 
            item_types[TYPE_ACORN_MOUTHFUL].emoji_name),
    .id = item_types[TYPE_ACORN_MOUTHFUL].emoji_id
  };
  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = u_snprintf(params->labels[0], sizeof(params->labels[0]), "Forage again!"),
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]), "%c%c_%ld",
        TYPE_FORAGE_INIT, ERROR_STATUS + 96, event->member->user->id),
    .emoji = &params->emojis[0]
  };

  params->emojis[1] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[1], sizeof(params->emoji_names[1]), 
            stats[STAT_STRENGTH].stat.emoji_name),
    .id = stats[STAT_STRENGTH].stat.emoji_id
  };
  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[1], sizeof(params->labels[1]), "Upgrades"),
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]), "%c_%ld",
        TYPE_UPGRADE, event->member->user->id),
    .emoji = &params->emojis[1]
  };

  params->emojis[2] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[2], sizeof(params->emoji_names[2]), 
            squirrels[player->squirrel].squirrel.emoji_name),
    .id = squirrels[player->squirrel].squirrel.emoji_id
  };
  params->buttons[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[2], sizeof(params->labels[2]), "Squirrels"),
    .custom_id = u_snprintf(params->custom_ids[2], sizeof(params->custom_ids[2]), "%c_%ld",
        TYPE_SQUIRREL, event->member->user->id),
    .emoji = &params->emojis[2]
  };

  struct sd_file_data biome_icon = biomes[player->biome].biome_icon;
  params->emojis[3] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[3], sizeof(params->emoji_names[3]), 
            biome_icon.emoji_name),
    .id = biome_icon.emoji_id
  };
  
  params->buttons[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[3], sizeof(params->labels[3]), "Info"),
    .custom_id = u_snprintf(params->custom_ids[3], sizeof(params->custom_ids[3]), "%c_%ld",
        TYPE_INFO_FROM_BUTTONS, event->member->user->id),
    .emoji = &params->emojis[3]
  };

  params->emojis[4] = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_names[4], sizeof(params->emoji_names[4]), 
            item_types[TYPE_NO_ACORNS].emoji_name),
    .id = item_types[TYPE_NO_ACORNS].emoji_id
  };
  
  params->buttons[4] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = u_snprintf(params->labels[4], sizeof(params->labels[4]), "Session Info"),
    .custom_id = u_snprintf(params->custom_ids[4], sizeof(params->custom_ids[4]), "%c_%ld",
        TYPE_SESSION_INFO, event->member->user->id),
    .emoji = &params->emojis[4]
  };

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
  if (event->emoji->id != items[ITEM_ACORNS].emoji_id)
    return;

  discord_add_guild_member_role(client, GUILD_ID, event->member->user->id, MEMBER_ROLE_ID, 
  &(struct discord_add_guild_member_role) { .reason = "New member" }, 
  NULL);
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
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
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
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, WELCOME_MSG_PATH)
    },
    .footer = &(struct discord_embed_footer) {
        .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
        .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ACORNS].file_path)
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