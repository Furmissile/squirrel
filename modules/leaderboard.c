PGresult* player_pos;

struct discord_components* build_leaderboard_buttons(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = 2;
  buttons->array = calloc(buttons->size, sizeof(struct discord_component));

  int button_idx = (event->data->custom_id) ? event->data->custom_id[1] - 48 : 0;

  buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 0) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "Acorn Count",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 0) ? true : false
  };

  buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 1) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "Courage",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c1_%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 1) ? true : false
  };

  return buttons;
}

void create_leaderboard_interaction(struct discord *client, const struct discord_interaction *event, struct sd_user_data *user_data) 
{
  struct sd_message *discord_msg = user_data->discord_msg;
  struct discord_embed *embed = discord_msg->embed;

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));

  for (int idx = 0; idx < user_data->db_rows; idx++)
  {
    struct DB_Info info = user_data->row_data[idx];
    if (player.user_id == info.user_id)
    {
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, 
        "**%d**. <@%ld> **%s** \n",
        info.db_idx, info.user_id, num_str(info.value) );

      user_data->is_top_ten = 1;
    }
    else
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, 
        "**%d**. `%s` **%s** \n",
        info.db_idx, info.username, num_str(info.value) );
  }

  PQclear(player_pos);
  player_pos = SQL_query(DB_ACTION_SEARCH, "select rank_idx, user_id, best_acorn \
        from ( \
          select user_id, \
          (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) AS best_acorn, \
          dense_rank() over (order by (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) desc) as rank_idx \
        from public.player) as lb where lb.user_id = %ld and lb.best_acorn > 0",
      player.user_id);

  if (user_data->is_top_ten == 0
    && PQntuples(player_pos) == 1) 
  {
    // player position in leaderboard
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
      "\n**%d**. <@%ld> **%s** \n",
      strtoint(PQgetvalue(player_pos, 0, 0)),
      strtobigint(PQgetvalue(player_pos, 0, 1)), 
      num_str(strtoint(PQgetvalue(player_pos, 0, 2))) );
  }

  complete_interaction(client, event, discord_msg);

  discord_embed_cleanup(embed);
  free(discord_msg);
  PQclear(player_pos);
}

void is_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct sd_user_data *user_data = resp->data;
  const struct discord_interaction *event = resp->keep;

  int idx = 0;
  // wait to match the id
  while (user_data->row_data[idx].user_id != user->id)
  {
    idx++;
    continue;
  }

  user_data->row_data[idx].username = format_str(SIZEOF_TITLE, user->username);

  user_data->response_counter++;

  if (user_data->response_counter == user_data->db_rows)
  {
    create_leaderboard_interaction(client, event, user_data);
    free(resp->data);
  }

}

void acorn_count_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct sd_message *discord_msg = resp->data;

  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Acorn Leaderboard");

  discord_msg->buttons = build_leaderboard_buttons(event);

  embed->color = player.color;

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORN_COUNT].item.file_path);

  struct sd_user_data *user_data = calloc(1, sizeof(struct sd_user_data));
  user_data->discord_msg = calloc(1, sizeof(struct sd_message));

  user_data->db_rows = PQntuples(player_pos);
  user_data->row_data = calloc(user_data->db_rows, sizeof(struct DB_Info));

  user_data->discord_msg = discord_msg;

  for (int idx = 0; idx < user_data->db_rows; idx++)
  {
    user_data->row_data[idx].db_idx = strtoint(PQgetvalue(player_pos, idx, 0));
    user_data->row_data[idx].user_id = strtobigint(PQgetvalue(player_pos, idx, 1));
    user_data->row_data[idx].value = strtoint(PQgetvalue(player_pos, idx, 2));
  
    struct discord_ret_user ret_user = {
      .done = &is_user,
      .data = user_data,
      .keep = event
    };

    discord_get_user(client, user_data->row_data[idx].user_id, &ret_user);
  }

}


void courage_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  struct sd_message *discord_msg = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Courage Leaderboard");

  discord_msg->buttons = build_leaderboard_buttons(event);

  embed->color = player.color;

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, scurry_items[SCURRY_ITEM_COURAGE].item.file_path);

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));

  int db_rows = PQntuples(player_pos);

  for (int idx = 0; idx < db_rows; idx++)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
      (player.scurry_id == strtobigint(PQgetvalue(player_pos, idx, 1))) 
          ? "%d. "GUILD_ICON" %s **%s** \n" : "%d. `%s` **%s** \n",
      strtoint(PQgetvalue(player_pos, idx, 0)), 
      PQgetvalue(player_pos, idx, 2), 
      num_str(strtoint(PQgetvalue(player_pos, idx, 3))) );

  complete_interaction(client, event, discord_msg);

  discord_embed_cleanup(embed);
  free(discord_msg);
}

int get_leaderboard(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg)
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  // discord_msg gets passed along regardless of leaderboard type
  struct discord_ret_interaction_response ret_response = { 
    .data = discord_msg, 
    .keep = event 
  };

  if (event->data->custom_id && event->data->custom_id[1] -48 == 1)
  {
    player_pos = SQL_query(DB_ACTION_SEARCH, "select dense_rank() over (order by courage desc) as rank_idx, owner_id, s_name, courage \
        from (select dense_rank() over (order by courage desc) as rank_idx, owner_id, s_name, courage from public.scurry) as lb \
        where rank_idx <= 10 and courage > 0");

    ERROR_DATABASE_RET((PQntuples(player_pos) == 0), "There aren't enough entries yet!", player_pos);

    ret_response.done = &courage_leaderboard;
  }
  else {
    player_pos = SQL_query(DB_ACTION_SEARCH, "select rank_idx, user_id, best_acorn \
        from ( \
          select user_id, \
          (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) AS best_acorn, \
          dense_rank() over (order by (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) desc) as rank_idx \
        from public.player) as lb where rank_idx <= 10 and best_acorn > 0");

    ERROR_DATABASE_RET((PQntuples(player_pos) == 0), "There aren't enough entries yet!", player_pos);

    ret_response.done = &acorn_count_leaderboard;
  }

  discord_create_interaction_response(client, event->id, event->token, 
    &(struct discord_interaction_response)
    {
      .type = (event->data->custom_id) ? DISCORD_INTERACTION_DEFERRED_UPDATE_MESSAGE : DISCORD_INTERACTION_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .content = format_str(SIZEOF_DESCRIPTION, "Loading leaderboard...")
      }
    },
    &ret_response);

  return 0;
}