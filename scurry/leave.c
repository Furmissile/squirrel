/*

  This file handles leaving a scurry on the player's prerogative

*/

int scurry_leave(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  ERROR_INTERACTION((player.scurry_id == 0), "You are not in a scurry yet!");

  ERROR_INTERACTION((scurry.war_flag == 1), "You cannot leave your scurry while at war! Ask the owner to retreat if you wish to leave.");

  ERROR_INTERACTION((player.scurry_id == player.user_id), "You can't leave your own scurry!");
  
  ERROR_INTERACTION((scurry.war_flag == 1), "You can't do this right now! Your scurry needs your help!");

  char* input = event->data->options->array[0].value;

  ERROR_INTERACTION((strcmp(lowercase(input), "yes") != 0), "Please enter 'yes' to verify!");

  player.scurry_id = 0;

  discord_msg->content = format_str(SIZEOF_DESCRIPTION, "<@!%ld>, <@!%ld> has left your scurry!", 
      scurry.scurry_owner_id, player.user_id);

  return 0;
}

int leave_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  if (scurry_leave(event, discord_msg) )
    return ERROR_STATUS;

  struct discord_interaction_response interaction = 
  {
    // function will always generate a new message
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = discord_msg->content,
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(player);

  free(discord_msg);

  return 0;
}