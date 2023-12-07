/*

  This file handles an owner kicking a member from their guild
  target user must
  - be a member of the scurry
  - not the owner
  - not their self
  - have scurry not at war

  - A DM is sent to the user who has been kicked with a mention
*/

struct sd_kick_info {
  struct sd_player *player;
  struct sd_scurry *scurry;

  unsigned long t_user_id;
};

// frees ALL pointers
void kick_info_cleanup(struct sd_kick_info *params)
{
  free(params->player);
  free(params->scurry);
  free(params);
}

// respond to owner's kick request
void create_kick_interaction(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct sd_kick_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  // kick user by setting id = 0
  PGresult* kick_scurry_member = (PGresult*) { 0 };
  kick_scurry_member = SQL_query(kick_scurry_member, 
      "update public.player set scurry_id = 0, stolen_acorns = 0 where user_id = %ld", 
      params->t_user_id);
  PQclear(kick_scurry_member);

  char msg_content[256] = { };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), 
          "**%s** has been kicked from **%s**!", 
          user->username, params->scurry->scurry_name),
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  kick_info_cleanup(params);
}

// send the kick message to the user via DM
void send_kick_dm(struct discord *client, struct discord_response *resp, const struct discord_channel *channel)
{
  struct sd_kick_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  char msg_content[256] = { };

  // creates message at dm channel
  discord_create_message(client, channel->id,
    &(struct discord_create_message)
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), "<@%ld>, you have been kicked from **%s**!", 
          params->t_user_id, params->scurry->scurry_name),
    }, 
    NULL);

  // request user data for username
  struct discord_ret_user ret_user = {
    .done = &create_kick_interaction,
    .fail = &not_user,
    .data = params,
    .keep = event
  };

  discord_get_user(client, params->t_user_id, &ret_user);
}

int kick_interaction(const struct discord_interaction *event)
{
  struct sd_kick_info *params = calloc(1, sizeof(struct sd_kick_info));

  params->player = calloc(1, sizeof(struct sd_player));
  load_player_struct(params->player, event->member->user->id, event->data->custom_id);

  struct sd_player *player = params->player;

  if (event->member->user->id != player->scurry_id)
  {
    error_message(event, "You don't have the permission to use this command!");
    free(params->player);
    free(params);
    return ERROR_STATUS;
  }

  params->scurry = calloc(1, sizeof(struct sd_scurry));
  load_scurry_struct(params->scurry, player->scurry_id);

  char user_id_buffer[64] = { };
  trim_user_id(user_id_buffer, sizeof(user_id_buffer), event->data->options->array[0].value);
  params->t_user_id = strtobigint(user_id_buffer);

  // if error, specify
  if (params->scurry->war_flag == 1)
  {
    error_message(event, "You cannot kick a member while at war! Please retreat or finish the war first.");
    kick_info_cleanup(params);
    return ERROR_STATUS;
  } 
  else if (params->t_user_id == event->member->user->id)
  {
    error_message(event, "You can't kick yourself, silly!");
    kick_info_cleanup(params);
    return ERROR_STATUS;
  }

  PGresult* target_user = (PGresult*) { 0 };
  target_user = SQL_query(target_user, "select * from public.player where user_id = %ld and scurry_id = %ld", 
      params->t_user_id, event->member->user->id);

  if (PQntuples(target_user) == 0)
  {
    error_message(event, "That user isn't a part of your scurry!");
    PQclear(target_user);
    kick_info_cleanup(params);
    return ERROR_STATUS;
  }

  PQclear(target_user);

  // get owner id for DM channel
  struct discord_create_dm dm_params = { .recipient_id = params->t_user_id };
  struct discord_ret_channel dm_ret = { 
    .done = &send_kick_dm,
    .data = params,
    .keep = event
  };

  // Return channel context for DM channel id
  discord_create_dm(client, &dm_params, &dm_ret);

  return 0;
}