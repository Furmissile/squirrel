struct sd_lb_data
{
  // set data in an array to prevent disorganization
  // must be calloc'd because size CANNOT be predicted (can be any value <= 10)
  struct sd_db_info *row_data;
  int db_rows;

  // how many requests have been completed
  int response_counter;

  int is_top_ten;
};

struct sd_leaderboard 
{
  struct sd_player *player;

  struct sd_lb_data *lb_data;

  PGresult* player_pos;

  struct discord_component buttons[2];
  char custom_ids[2][64];

  char description[1024];
};


void create_leaderboard_interaction(const struct discord_interaction *event, struct sd_leaderboard *params)
{
  struct sd_player *player = params->player;

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player->color,
    .title = "Leaderboard",
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .description = params->description
  };

  int button_idx = (event->data->custom_id) ? event->data->custom_id[1] - 48 : 0;

  header.embed.thumbnail = &(struct discord_embed_thumbnail) {
    .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
        (button_idx == 0) ? items[ITEM_ACORN_COUNT].file_path : scurry_items[SCURRY_ITEM_WAR_ACORNS].file_path)
  };

  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 0) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "Acorn Count",
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]),
        "%c0_%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 0) ? true : false
  };

  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 1) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "War Acorns",
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]),
        "%c1_%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 1) ? true : false
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params->buttons,
      .size = 2
    }
  };

  struct discord_edit_original_interaction_response interaction = 
  {
    .embeds = &(struct discord_embeds) 
    {
      .array = &header.embed,
      .size = 1
    },
    .components = &(struct discord_components) {
      .array = &action_rows,
      .size = 1
    }
  };

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, &interaction, NULL);

  char values[16384];
  CCORDcode code = discord_edit_original_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  free(params->player);
  // not needed for the total war acorns leaderboard
  if (params->lb_data)
  {
    free(params->lb_data->row_data);
    free(params->lb_data);
  }
  free(params);
}

void is_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  struct sd_leaderboard *params = resp->data;
  struct sd_lb_data *lb_data = params->lb_data;
  const struct discord_interaction *event = resp->keep;

  struct sd_player *player = params->player;

  u_snprintf(lb_data->row_data[lb_data->response_counter].username, 
      sizeof(lb_data->row_data[lb_data->response_counter].username), 
      user->username);

  lb_data->response_counter++;

  if (lb_data->response_counter == lb_data->db_rows)
  {
    for (int lb_idx = 0; lb_idx < lb_data->db_rows; lb_idx++)
    {
      struct sd_db_info info = lb_data->row_data[lb_idx];
      APPLY_NUM_STR(value, info.value);

      if (player->user_id == info.user_id)
      {
        u_snprintf(params->description, sizeof(params->description),
            "**%d**. <@%ld> **%s** \n",
            info.db_idx, info.user_id, value);
        
        lb_data->is_top_ten = 1;
      }
      else {
        u_snprintf(params->description, sizeof(params->description),
        "**%d**. `%s` **%s** \n",
        info.db_idx, info.username, value);
      }
    }

    PQclear(params->player_pos);
    params->player_pos = SQL_query(params->player_pos, 
        "select rank_idx, user_id, best_acorn "
        "from ( "
        "select user_id, "
        "(CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) AS best_acorn, "
        "dense_rank() over (order by (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) desc) as rank_idx "
        "from public.player) as lb where lb.user_id = %ld and lb.best_acorn > 0",
        player->user_id);
    
    if (lb_data->is_top_ten == 0
      && PQntuples(params->player_pos) == 1)
    {
      APPLY_NUM_STR( value, strtoint(PQgetvalue(params->player_pos, 0, 2)) );
      u_snprintf(params->description, sizeof(params->description),
          "\n**%d**. <@%ld> **%s** \n",
          strtoint(PQgetvalue(params->player_pos, 0, 0)),
          strtobigint(PQgetvalue(params->player_pos, 0, 1)), value);
    }
    create_leaderboard_interaction(event, params);
  }
}

void acorn_count_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct sd_leaderboard *params = resp->data;

  // calloc leaderboard data
  params->lb_data = calloc(1, sizeof(struct sd_lb_data));
  struct sd_lb_data *lb_data = params->lb_data;

  // calloc individual rows
  int row_count = PQntuples(params->player_pos);
  lb_data->db_rows = (row_count < 10) ? row_count : 10;
  lb_data->row_data = calloc(lb_data->db_rows, sizeof(struct sd_db_info));

  for (int user_idx = 0; user_idx < lb_data->db_rows; user_idx++)
  {
    lb_data->row_data[user_idx].db_idx = strtoint(PQgetvalue(params->player_pos, user_idx, 0));
    lb_data->row_data[user_idx].user_id = strtobigint(PQgetvalue(params->player_pos, user_idx, 1));
    lb_data->row_data[user_idx].value = strtoint(PQgetvalue(params->player_pos, user_idx, 2));

    struct discord_ret_user ret_user = {
      .done = &is_user,
      .data = params,
      .keep = event
    };

    discord_get_user(client, lb_data->row_data[user_idx].user_id, &ret_user);
  }
}

void war_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)client;
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct sd_leaderboard *params = resp->data;

  struct sd_player *player = params->player;

  int db_rows = PQntuples(params->player_pos);
  for (int user_idx = 0; user_idx < db_rows; user_idx++)
  {
    APPLY_NUM_STR( value, strtoint(PQgetvalue(params->player_pos, user_idx, 3)) );
    u_snprintf(params->description, sizeof(params->description),
      ( player->scurry_id == strtobigint(PQgetvalue(params->player_pos, user_idx, 1)) ) 
          ? "**%d**. "GUILD_ICON" %s **%s** \n"
          : "**%d**. `%s` **%s** \n",
      strtoint(PQgetvalue(params->player_pos, user_idx, 0)), 
      PQgetvalue(params->player_pos, user_idx, 2), value);
  }
  create_leaderboard_interaction(event, params);
}

int fetch_leaderboard(const struct discord_interaction *event)
{
  struct sd_leaderboard *params = calloc(1, sizeof(struct sd_leaderboard));

  params->player = calloc(1, sizeof(struct sd_player));
  load_player_struct(params->player, event->member->user->id);

  // sd_leaderboard gets passed along regardless of leaderboard type
  struct discord_ret_interaction_response ret_response = { 
    .data = params,
    .keep = event
  };

  params->player_pos = (PGresult*) { 0 };
  if (event->data->custom_id && event->data->custom_id[1] -48 == 1)
  {
    params->player_pos = SQL_query(params->player_pos,
      "select rank_idx, owner_id, s_name, best_acorn "
      "from ("
      "select owner_id, s_name, "
      "(CASE WHEN prev_stolen_acorns > total_stolen_acorns THEN prev_stolen_acorns ELSE total_stolen_acorns END) AS best_acorn, "
      "dense_rank() over (order by (CASE WHEN prev_stolen_acorns > total_stolen_acorns THEN prev_stolen_acorns ELSE total_stolen_acorns END) desc) as rank_idx "
      "from public.scurry) as lb where rank_idx <= 10 and best_acorn > 0");

    ret_response.done = &war_leaderboard;
  }
  else {
    params->player_pos = SQL_query(params->player_pos, 
      "select rank_idx, user_id, best_acorn "
      "from ("
      "select user_id, "
      "(CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) AS best_acorn, "
      "dense_rank() over (order by (CASE WHEN high_acorn_count > acorn_count THEN high_acorn_count ELSE acorn_count END) desc) as rank_idx "
      "from public.player) as lb where rank_idx <= 10 and best_acorn > 0");

    ret_response.done = &acorn_count_leaderboard;
  }

  if (PQntuples(params->player_pos) == 0)
  {
    error_message(event, "There aren't enough entries yet!");
    PQclear(params->player_pos);
    free(params->player);
    free(params);
    return ERROR_STATUS;
  }

  discord_create_interaction_response(client, event->id, event->token, 
    &(struct discord_interaction_response)
    {
      .type = (event->data->custom_id) ? DISCORD_INTERACTION_DEFERRED_UPDATE_MESSAGE : DISCORD_INTERACTION_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .content = "Loading leaderboard..."
      }
    },
    &ret_response);
  
  return 0;
}