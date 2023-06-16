
struct sd_steal_info {
  unsigned long t_user_id;
  int steal_amt;
  int golden_acorns;

  char username[64];

  struct sd_player *player;

  char content[256];
  char description[512];

  char footer_text[64];
  char footer_url[128];
};

enum TARGET_USER {
  TARGET_USER_ID,
  TARGET_USER_ACORNS
};

void steal_acorns(
  const struct discord_interaction *event, 
  struct sd_steal_info *params)
{
  struct sd_player *player = params->player;

  struct sd_header_params header = { 0 };

  int color = 0;
  APPLY_NUM_STR(steal_amt, params->steal_amt);

  if (rand() % MAX_CHANCE > STEAL_CHANCE)
  {
    color = (int)ACTION_FAILED;
    u_snprintf(header.title, sizeof(header.title), "Steal Failed!");

    u_snprintf(params->content, sizeof(params->content), 
        "**%s**, someone failed to snatch your acorns!", params->username);
    
    u_snprintf(params->description, sizeof(params->description),
        "<@%ld> failed to steal **%s** "ACORNS" acorns! \n"
        "\n-**20** "ENERGY" Energy \n", 
        event->member->user->id, steal_amt);
  }
  else {
    color = (int)ACTION_SUCCESS;
    u_snprintf(header.title, sizeof(header.title), "Steal Successful!");

    PGresult* search_player = (PGresult*) { 0 };
    SQL_query(search_player, "update public.player set acorns = acorns - %d where user_id = %ld", 
        params->steal_amt, params->t_user_id);
    PQclear(search_player);

    player->acorns += params->steal_amt;
    player->golden_acorns += params->golden_acorns;

    APPLY_NUM_STR(golden_acorns, params->golden_acorns);

    u_snprintf(params->description, sizeof(params->description),
        "You anonymously stole **%s** "ACORNS" acorns! \n"
        "\n+**%s** "GOLDEN_ACORNS" Golden Acorns \n"
        "\n-**20** "ENERGY" Energy \n",
        steal_amt, golden_acorns);
  }
  
  player->energy -= STEAL_ENERGY_COST;
  
  struct discord_embed *embed = &header.embed;

  embed->color = color;

  embed->author = &(struct discord_embed_author) 
  {
    .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
    .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
        "https://cdn.discordapp.com/avatars/%lu/%s.png",
        event->member->user->id, event->member->user->avatar)
  };

  embed->description = params->description;

  embed->footer = &(struct discord_embed_footer) 
  {
    .text = u_snprintf(params->footer_text, sizeof(params->footer_text), 
        "You have %d energy left!", player->energy),
    .icon_url = u_snprintf(params->footer_url, sizeof(params->footer_url), 
        GIT_PATH, items[ITEM_ENERGY].file_path)
  };

  player->main_cd = time(NULL) + COOLDOWN;

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = params->content,
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(params->player);

  free(params->player);
  free(params);
}

/* if user, format string using backticks */
void steal_from_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  struct sd_steal_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  u_snprintf(params->username, sizeof(params->username), "`%s`", user->username);

  steal_acorns(event, params);
}

/* if not member, fetch user */
void steal_not_member(struct discord *client, struct discord_response *resp)
{
  struct sd_steal_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_ret_user ret_user = {
    .done = &steal_from_user,
    .data = params,
    .keep = event
  };

  discord_get_user(client, params->t_user_id, &ret_user);
}

/* if member, format string using mention */
void steal_from_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  (void)client;
  struct sd_steal_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  u_snprintf(params->username, sizeof(params->username), "<@%ld>", member->user->id);

  steal_acorns(event, params);
}

int steal_interaction(const struct discord_interaction *event) 
{
  struct sd_player *player = calloc(1, sizeof(struct sd_player));
  load_player_struct(player, event->member->user->id);

  energy_regen(player);
  ERROR_INTERACTION((time(NULL) < player->main_cd), "Cooldown not ready!");
  ERROR_INTERACTION((player->energy < STEAL_ENERGY_COST), "You need more energy!");

  float random_percent = (float)genrand(15, 5) /100;

  int steal_min = generate_price(player->stats.proficiency_lv, PROFICIENCY_UNIT);

  int relative_golden_acorns = GOLDEN_ACORN_BUFF_COST * generate_factor(player->stats.luck_lv, LUCK_FACTOR);
  
  // select all players that isnt the player, game owner, within a range of acorns, or is the same scurry
  PGresult* t_user = (PGresult*) { 0 };
  
  // Issue: PQntuples returns no rows but psql returns one
  if (player->scurry_id > 0)
  {
    t_user = SQL_query(t_user, 
        "select user_id, acorns from public.player where user_id != %ld and acorns > %d and scurry_id != %ld order by random() LIMIT 1", 
        event->member->user->id, steal_min, player->scurry_id);
  }
  else {
    t_user = SQL_query(t_user, 
        "select user_id, acorns from public.player where user_id != %ld and acorns > %d order by random() LIMIT 1", 
        event->member->user->id, steal_min);
  }
  
  ERROR_DATABASE_RET((PQntuples(t_user) == 0), "No targets are available right now!", t_user);

  struct sd_steal_info *params = calloc(1, sizeof(struct sd_steal_info));
  params->player = player;

  params->t_user_id = strtobigint( PQgetvalue(t_user, 0, TARGET_USER_ID));

  // take a random percent of steal min
  params->steal_amt = steal_min * random_percent;
  params->golden_acorns = relative_golden_acorns * random_percent;

  struct discord_ret_guild_member ret_member = {
    .done = &steal_from_member,
    .fail = &steal_not_member,
    .data = params,
    .keep = event
  };

  PQclear(t_user);

  // check if guild member
  discord_get_guild_member(client, event->guild_id, params->t_user_id, &ret_member);

  return 0;
}