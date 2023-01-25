/*

  This file handles acorn stealing
  - A random player is selected from the query
  - Players steal from other players based on proficiency stat and a FIXED chance
  - No need to set acorns to 0 if less than since the amount stolen is a percent of their acorns
  - If the steal failed, the player is notified, otherwise the steal is anonymous!
  - Players cannot steal below a certain amount of acorns on success
  - If the target is not a member of the guild, their username is used instead of a mention

*/

struct Steal_Info {
  unsigned long t_user_id;
  int t_acorns;

  struct sd_message *discord_msg;
  char* username;
};

// Instead of enumerating, create a struct to allocate
enum TARGET_USER {
  TARGET_USER_ID,
  TARGET_USER_ACORNS
};

void create_steal_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = discord_msg->content,

      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg);

  update_player_row(player);
  rewards = (struct sd_rewards) { 0 };
}

void steal_acorns(
  const struct discord_interaction *event, 
  struct Steal_Info *steal_info)
{
  struct sd_message *discord_msg = steal_info->discord_msg;

  struct discord_embed *embed = discord_msg->embed;

  //Load Author
  embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  // players can steal anywhere between 25 and 50% of their said value
  float random_percent = (float)genrand(25, 25) /100;
  rewards.stolen_acorns = steal_info->t_acorns * random_percent;

  if (rand() % MAX_CHANCE > STEAL_CHANCE)
  {
    embed->color = (int)ACTION_FAILED;
    embed->title = format_str(SIZEOF_TITLE, "Steal Failed!");

    discord_msg->content = format_str(SIZEOF_DESCRIPTION, "**%s**, someone failed to snatch your acorns!", steal_info->username);

    embed->description = format_str(SIZEOF_DESCRIPTION, 
        "<@%ld> failed to steal **%s** "ACORNS" acorns! \n", 
        event->member->user->id, num_str(rewards.stolen_acorns));
  }
  else {
    SQL_query(conn, "update public.player set acorns = %d where user_id = %ld", 
        steal_info->t_acorns - rewards.stolen_acorns, steal_info->t_user_id);

    rewards.golden_acorns = genrand(25, 25) * generate_factor(player.stats.luck_lv, LUCK_VALUE);

    player.acorns += rewards.stolen_acorns;
    player.acorn_count += rewards.stolen_acorns * 0.1;
    player.golden_acorns += rewards.golden_acorns;

    embed->color = (int)ACTION_SUCCESS;
    embed->title = format_str(SIZEOF_TITLE, "Steal Successful!");
    embed->description = format_str(SIZEOF_DESCRIPTION,
        "You anonymously stole **%s** "ACORNS" acorns! \n"
        "+**%s** "GOLDEN_ACORNS" Golden Acorns \n",
        num_str(rewards.stolen_acorns), num_str(rewards.golden_acorns));
  }

  free(steal_info);
  
  energy_status(discord_msg, STEAL_ENERGY_COST);

  player.main_cd = time(NULL) + COOLDOWN;

  create_steal_interaction(event, discord_msg);
}

/* if user, format string using backticks */
void steal_from_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  steal_info->username = format_str(SIZEOF_TITLE, "`%s`", user->username);

  steal_acorns(event, steal_info);
}

/* if not member, fetch user */
void steal_not_member(struct discord *client, struct discord_response *resp)
{
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_ret_user ret_user = {
    .done = &steal_from_user,
    .data = steal_info,
    .keep = event
  };

  discord_get_user(client, steal_info->t_user_id, &ret_user);
}

/* if member, format string using mention */
void steal_from_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  (void)client;
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  steal_info->username = format_str(SIZEOF_TITLE, "<@%ld>", member->user->id);

  steal_acorns(event, steal_info);
}

/* Listens for slash command interactions */
int steal_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  energy_regen();
  ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready!");
  ERROR_INTERACTION((player.energy < STEAL_ENERGY_COST), "You need more energy!");

  int steal_max = UNIT_ACORN * (*stats[STAT_PROFICIENCY].stat_ptr +1);

  // select all players that isnt the player, game owner, within a range of acorns, or is the same scurry
  PGresult* t_user = SQL_query(conn, "select user_id, acorns from public.player \
      where user_id != %ld \
      and user_id != %ld \
      %s \
      and acorns > %d \
      and acorns < %d \
      order by random() LIMIT 1", 
      event->member->user->id, OWNER_ID,
      (player.scurry_id > 0) ? format_str(SIZEOF_URL, "and scurry_id != %ld", player.scurry_id) : "",
      steal_max, steal_max *2 );

  ERROR_DATABASE_RET((PQntuples(t_user) == 0), "No target users are available right now!", t_user);

  struct Steal_Info *steal_info = calloc(1, sizeof(struct Steal_Info));
  steal_info->discord_msg = discord_msg;

  steal_info->t_user_id = strtobigint( PQgetvalue(t_user, 0, TARGET_USER_ID));
  steal_info->t_acorns = strtoint( PQgetvalue(t_user, 0, TARGET_USER_ACORNS) );

  struct discord_ret_guild_member ret_member = {
    .done = &steal_from_member,
    .fail = &steal_not_member,
    .data = steal_info,
    .keep = event
  };

  // check if guild member
  discord_get_guild_member(client, event->guild_id, steal_info->t_user_id, &ret_member);

  return 0;
}