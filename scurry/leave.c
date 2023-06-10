struct sd_leave_info {
  struct sd_scurry *scurry;

  unsigned long owner_id;
};

void leave_info_cleanup(struct sd_leave_info *params)
{
  free(params->scurry);
  free(params);
}

void create_leave_interaction(const struct discord_interaction *event, struct sd_leave_info *params)
{
  PGresult* scurry_member_leave = (PGresult*) { 0 };
  scurry_member_leave = SQL_query(scurry_member_leave, 
      "update public.player set scurry_id = 0, stolen_acorns = 0 where user_id = %ld", 
      event->member->user->id);
  PQclear(scurry_member_leave);

  char msg_content[256] = { 0 };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = u_snprintf(msg_content, sizeof(msg_content), 
          "<@%ld>, you have left **%s**!", 
          event->member->user->id, params->scurry->scurry_name),
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  leave_info_cleanup(params);
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
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  struct sd_leave_info *params = calloc(1, sizeof(struct sd_leave_info));

  params->scurry = calloc(1, sizeof(struct sd_scurry));
  load_scurry_struct(params->scurry, player.scurry_id);

  struct sd_scurry *scurry = params->scurry;

  params->owner_id = player.scurry_id;

  char* input = event->data->options->array[0].value;
  size_t input_len = strlen(input);

  if (player.scurry_id == 0)
  {
    error_message(event, "You are not in a scurry yet!");
    leave_info_cleanup(params);
  }
  else if (scurry->war_flag == 1)
  {
    error_message(event, "You cannot leave your scurry while at war! Ask the owner to retreat if you wish to leave.");
    leave_info_cleanup(params);
  }
  else if (player.scurry_id == player.user_id)
  {
    error_message(event, "You can't leave your own scurry, silly!");
    leave_info_cleanup(params);
  }
  else if (input_len != 3)
  {
    error_message(event, "Please enter 'yes' to verify!");
    leave_info_cleanup(params);
  }
  else {
    // strlen is checked before to prevent buffer overflow
    char lowercase_input[32] = { };
    lowercase(lowercase_input, sizeof(lowercase_input), input);
    ERROR_INTERACTION((strcmp(lowercase_input, "yes") != 0), "Please enter 'yes' to verify!");

    PGresult* leave_cmd = (PGresult*) { 0 };
    leave_cmd = SQL_query(leave_cmd, "update public.player set scurry_id = 0 where user_id = %ld",
        event->member->user->id);
    PQclear(leave_cmd);

    // send DM to the owner
    struct discord_create_dm dm_params = { .recipient_id = params->owner_id };
    struct discord_ret_channel dm_ret = { 
      .done = &send_leave_dm,
      .data = params,
      .keep = event
    };

    // Return channel context for DM channel id
    discord_create_dm(client, &dm_params, &dm_ret);
  }

  return 0;
}
