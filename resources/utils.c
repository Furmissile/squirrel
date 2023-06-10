
/* Returns total factor based on stat level and if it's a multiplier or incrementer */
float generate_factor(int stat_lv, double value_factor)
{
  if (value_factor == STRENGTH_FACTOR)
    return value_factor * (stat_lv -1);
  else
    return (value_factor * (stat_lv -1)) + (value_factor * (stat_lv/STAT_EVOLUTION)) +1;
}

/* Returns a total price based on stat level, currency unit, and a value multiplier */
int generate_price(int stat_lv, double value_mult)
{
  // return (value_mult * stat_lv) + (value_mult * (stat_lv/STAT_EVOLUTION));
  return ((stat_lv +1)/STAT_EVOLUTION * value_mult + value_mult) * stat_lv;
}

int energy_status(char* sd_description, size_t description_size, struct sd_player *player, int energy_loss)
{
  if (player->squirrel == GRAY_SQUIRREL
    && rand() % MAX_CHANCE > 60
    && energy_loss == MAIN_ENERGY_COST) 
  {
    u_snprintf(sd_description, description_size, "\n "ENERGY" No energy was lost! \n");
    return 0;
  }

  player->energy -= energy_loss;

  u_snprintf(sd_description, description_size, "\n-**%d** "ENERGY" Energy \n", energy_loss);

  return energy_loss;
}

void energy_regen(struct sd_player *player) 
{
  if (player->buffs.endurance_acorn > 0)
  {
    player->energy += player->buffs.endurance_acorn;
    player->buffs.endurance_acorn = 0;
  }
  if (player->buffs.strength_acorn > 0)
    player->buffs.strength_acorn = 0;

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
      rewards->seasoned_golden_acorns = (rewards->item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
          : (rewards->item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);

      rewards->seasoned_golden_acorns *= generate_factor(player->stats.luck_lv, LUCK_FACTOR);
      
      player->golden_acorns += rewards->seasoned_golden_acorns;
    }
    return SPRING_MULT;
  }
  else if (info->tm_mday < 14) 
  {
    if (rand() % MAX_CHANCE > 80)
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
      }
      else if (victuals_chance < CHERRY_CHANCE) 
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
  u_snprintf(sd_description, description_size, "\n+**%s** "ACORNS" Acorns (+**%s** "ACORN_COUNT" Acorn Count) \n%s", 
      acorns, acorn_count, (buff_status->proficiency_acorn) ? "-**1** "PROFICIENCY_ACORN" Acorn of Proficiency \n" : " " );

  // GOLDEN ACORNS
  if (rewards->golden_acorns) 
  {
    APPLY_NUM_STR(reward, rewards->golden_acorns);

    u_snprintf(sd_description, description_size, "\n+**%s** "GOLDEN_ACORNS" Golden Acorns \n%s", 
        reward, (buff_status->luck_acorn) ? "-**1** "LUCK_ACORN" Acorn of Luck \n" : " " );
  }

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
    u_snprintf(sd_description, description_size, "\n+**%d** "CONJURED_ACORNS" Conjured Acorns \n", rewards->conjured_acorns);
  // if there's war acorns, there's courage
  else if (rewards->stolen_acorns)
  {
    APPLY_NUM_STR(stolen_acorns, rewards->stolen_acorns);
    u_snprintf(sd_description, description_size, "\n+**%s** "WAR_ACORNS" Stolen Acorns! \n", stolen_acorns);
  }
  else if (rewards->war_acorns)
    u_snprintf(sd_description, description_size, "\n+**%d** "WAR_ACORNS" War Acorns \n", rewards->war_acorns);

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
  rewards->acorns += (BIOME_ACORN_INC * player->biome_num);

  if (player->vengeance_flag)
    rewards->acorns *= 2;

  // apply proficiency acorn before setting acorn count!
  if (player->buffs.proficiency_acorn) {
    player->buffs.proficiency_acorn--;
    rewards->acorns *= 2;
    buff_status->proficiency_acorn = true;
  }

  rewards->acorn_count = rewards->acorns;
  player->acorn_count += rewards->acorn_count;

  // stat is not included in acorn count
  rewards->acorns *= generate_factor(player->stats.proficiency_lv, PROFICIENCY_FACTOR);

  // season bonus always applies to acorns -- golden acorns are handled separately
  // season bonuses must always be displayed last on rewards
  if (rewards->item_type != TYPE_NO_ACORNS
    && rewards->item_type != TYPE_HEALTH_LOSS)
  {
    int season_factor = factor_season(player, rewards);
    rewards->acorns *= season_factor;
  }

  player->acorns += rewards->acorns;

  // GOLDEN ACORNS
  if (rewards->golden_acorns) {
    rewards->golden_acorns += (BIOME_GOLDEN_ACORN_INC * player->biome_num);
  
    if (player->vengeance_flag)
      rewards->golden_acorns *= 2;

    rewards->golden_acorns *= generate_factor(player->stats.luck_lv, LUCK_FACTOR);

    if (player->buffs.luck_acorn > 0) {
      player->buffs.luck_acorn--;
      rewards->golden_acorns *= 2;
      buff_status->luck_acorn = true;
    }

    player->golden_acorns += rewards->golden_acorns;
  }

}

int get_season_event(const struct discord_interaction *event)
{
  struct tm *info = get_UTC();

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char* seasons[4] = {"Spring (Spring Chicken Live!)", "Summer (Garden Raid Season!)", "Fall (Winter Prep Live!)", "Winter (Bunny's Endeavor Live!)"};

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