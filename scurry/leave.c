struct sd_leave_info {
  struct sd_player *player;
  struct sd_scurry *scurry;

  unsigned long owner_id;
};

void leave_info_cleanup(struct sd_leave_info *params)
{
  free(params->scurry);
  free(params->player);
  free(params);
}

void create_leave_interaction(const struct discord_interaction *event, struct sd_leave_info *params)
{
  PGresult* scurry_member_leave = (PGresult*) { 0 };
  scurry_member_leave = SQL_query(scurry_member_leave, 
      "update public.player set scurry_id = 0, stolen_acorns = 0 where user_id = %ld", 
      params->player->user_id);
  PQclear(scurry_member_leave);

  char msg_content[256] = { 0 };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), 
          "<@%ld>, you have left **%s**!", 
          params->player->user_id, params->scurry->scurry_name),
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  leave_info_cleanup(params);

  // go back to info!
  info_from_buttons(event);
}

void send_leave_dm(struct discord *client, struct discord_response *resp, const struct discord_channel *channel)
{
  struct sd_leave_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  char msg_content[256] = { 0 };

  // creates message at dm channel
  discord_create_message(client, channel->id,
    &(struct discord_create_message)
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), "<@%ld>, **%s** has left your scurry!",
          params->owner_id, event->member->user->username)
    }, 
    NULL);
  
  create_leave_interaction(event, params);
}

int leave_interaction(const struct discord_interaction *event)
{
  struct sd_leave_info *params = calloc(1, sizeof(struct sd_leave_info));

  params->player = calloc(1, sizeof(struct sd_player));
  load_player_struct(params->player, event->member->user->id, event->data->custom_id);
  struct sd_player *player = params->player;

  params->scurry = calloc(1, sizeof(struct sd_scurry));
  load_scurry_struct(params->scurry, player->scurry_id);
  struct sd_scurry *scurry = params->scurry;

  params->owner_id = player->scurry_id;

  if (event->message->timestamp /1000 < player->timestamp)
  {
    error_message(event, "This appears to be an old session! Please renew your session by sending `/start`.");
    leave_info_cleanup(params);
    return ERROR_STATUS;
  }
  else if (player->scurry_id == 0)
  {
    error_message(event, "You are not in a scurry!");
    leave_info_cleanup(params);
    return ERROR_STATUS;
  }
  else if (scurry->war_flag == 1)
  {
    error_message(event, "You cannot leave your scurry while at war! Please wait until the war is finished or the leader has retreated.");
    leave_info_cleanup(params);
    return ERROR_STATUS;
  }
  else if (player->scurry_id == player->user_id)
  {
    error_message(event, "You can't leave your own scurry, silly!");
    leave_info_cleanup(params);
    return ERROR_STATUS;
  }
  else {
    PGresult* leave_cmd = (PGresult*) { 0 };
    leave_cmd = SQL_query(leave_cmd, "update public.player set scurry_id = 0 where user_id = %ld",
        player->user_id);
    PQclear(leave_cmd);

    PGresult *check_leave = (PGresult*) { 0 };
    check_leave = SQL_query(check_leave, "select * from player where user_id = %ld and scurry_id = %ld",
        player->user_id, player->scurry_id);

    PGresult* check_scurry = (PGresult*) { 0 };
    DATABASE_ERROR((PQntuples(check_scurry) > 0),
        "An error has occurred while leaving this scurry. \n"
        "Please report this error to the support server! (link on last page of player help)", check_scurry);
    PQclear(check_scurry);

    // send DM to the owner
    struct discord_create_dm dm_params = { .recipient_id = params->owner_id };
    struct discord_ret_channel dm_ret = { 
      .done = &send_leave_dm,
      .data = params,
      .keep = event
    };

    player->timestamp = event->message->timestamp /1000;

    // Return channel context for DM channel id
    discord_create_dm(client, &dm_params, &dm_ret);
  }

  return 0;
}
