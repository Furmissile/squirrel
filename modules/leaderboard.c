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

enum PLAYER_LB_LEADERBOARD
{
  PLAYER_LB_RANK_IDX,
  PLAYER_LB_USER_ID,
  PLAYER_LB_HIGH_ACORN_COUNT,
  PLAYER_LB_SIZE
};

enum SCURRY_LB_LEADERBOARD
{
  SCURRY_LB_RANK_IDX,
  SCURRY_LB_OWNER_ID,
  SCURRY_LB_NAME,
  SCURRY_LB_TOTAL_STOLEN_ACORNS,
  SCURRY_LB_SIZE
};

void leaderboard_info_cleanup(struct sd_leaderboard *params)
{
  free(params->player);
  // not needed for the total war acorns leaderboard
  if (params->lb_data)
  {
    free(params->lb_data->row_data);
    free(params->lb_data);
  }
  free(params);
}

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
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .description = params->description
  };

  int button_idx = (event->data->custom_id) ? event->data->custom_id[1] - 48 : 0;

  header.embed.thumbnail = &(struct discord_embed_thumbnail) {
    .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
        (button_idx == 0) ? slice_types[TYPE_ACORN_COUNT].item.file_path : slice_types[TYPE_WAR_ACORNS].item.file_path)
  };

  params->buttons[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 0) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "Acorn Count",
    .custom_id = u_snprintf(params->custom_ids[0], sizeof(params->custom_ids[0]),
        "%c0.%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 0) ? true : false
  };

  params->buttons[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = (button_idx == 1) ? DISCORD_BUTTON_SECONDARY : DISCORD_BUTTON_PRIMARY,
    .label = "War Acorns",
    .custom_id = u_snprintf(params->custom_ids[1], sizeof(params->custom_ids[1]),
        "%c1.%ld", TYPE_LEADERBOARD, event->member->user->id),
    .disabled = (button_idx == 1) ? true : false
  };

  struct sd_statistics stats = { 0 };

  init_statistics_buttons(event, &stats, player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, player, &util_data);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params->buttons,
        .size = 2
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = stats.buttons,
        .size = sizeof(stats.buttons)/sizeof(*stats.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = util_data.buttons_displayed
      }
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
      .array = action_rows,
      .size = 3
    }
  };

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, &interaction, NULL);

  update_player_row(params->player);

  leaderboard_info_cleanup(params);
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
        "select rank_idx, user_id, high_acorn_count "
        "from ("
        "select user_id, "
        "high_acorn_count, "
        "dense_rank() over (order by high_acorn_count desc) as rank_idx "
        "from public.player) as lb where user_id = %ld",
        player->user_id);
    
    if (lb_data->is_top_ten == 0)
    {
      APPLY_NUM_STR( value, strtoint(PQgetvalue(params->player_pos, 0, PLAYER_LB_HIGH_ACORN_COUNT)) );
      u_snprintf(params->description, sizeof(params->description),
          "\n**%d**. <@%ld> **%s** \n",
          strtoint(PQgetvalue(params->player_pos, 0, PLAYER_LB_RANK_IDX)),
          strtobigint(PQgetvalue(params->player_pos, 0, PLAYER_LB_USER_ID)), 
          value);
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
    lb_data->row_data[user_idx].db_idx = strtoint(PQgetvalue(params->player_pos, user_idx, PLAYER_LB_RANK_IDX));
    lb_data->row_data[user_idx].user_id = strtobigint(PQgetvalue(params->player_pos, user_idx, PLAYER_LB_USER_ID));
    lb_data->row_data[user_idx].value = strtoint(PQgetvalue(params->player_pos, user_idx, PLAYER_LB_HIGH_ACORN_COUNT));

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
    APPLY_NUM_STR( value, strtoint(PQgetvalue(params->player_pos, user_idx, SCURRY_LB_TOTAL_STOLEN_ACORNS)) );

    u_snprintf(params->description, sizeof(params->description),
      ( player->scurry_id == strtobigint(PQgetvalue(params->player_pos, user_idx, 1)) ) 
          ? "**%d**. "GUILD_ICON" %s **%s** \n"
          : "**%d**. `%s` **%s** \n",
      strtoint(PQgetvalue(params->player_pos, user_idx, SCURRY_LB_RANK_IDX)), 
      PQgetvalue(params->player_pos, user_idx, SCURRY_LB_NAME), 
      value);
  }

  create_leaderboard_interaction(event, params);
}

int fetch_leaderboard(const struct discord_interaction *event)
{
  struct sd_leaderboard *params = calloc(1, sizeof(struct sd_leaderboard));

  params->player = calloc(1, sizeof(struct sd_player));
  load_player_struct(params->player, event->member->user->id, event->data->custom_id);
  struct sd_player *player = params->player;

  if (event->message->timestamp /1000 < player->timestamp)
  {
    error_message(event, "This appears to be an old session! Please renew your session by sending `/start`.");
    leaderboard_info_cleanup(params);
    return ERROR_STATUS;
  }

  params->player->timestamp = event->message->timestamp /1000;

  // sd_leaderboard gets passed along regardless of leaderboard type
  struct discord_ret_interaction_response ret_response = { 
    .data = params,
    .keep = event
  };

  params->player_pos = (PGresult*) { 0 };
  if (event->data->custom_id && params->player->button_idx == 1)
  {
    params->player_pos = SQL_query(params->player_pos,
        "select rank_idx, owner_id, name, total_stolen_acorns "
        "from ("
        "select owner_id, name, "
        "SUM(stolen_acorns) as total_stolen_acorns, "
        "dense_rank() over (order by SUM(stolen_acorns) desc) as rank_idx "
        "from public.player join public.scurry on player.scurry_id = scurry.owner_id group by owner_id, name) "
        "as lb where total_stolen_acorns > 0 LIMIT 10");

    ret_response.done = &war_leaderboard;
  }
  else {
    params->player_pos = SQL_query(params->player_pos, 
        "select rank_idx, user_id, high_acorn_count "
        "from ("
        "select user_id, "
        "high_acorn_count, "
        "dense_rank() over (order by high_acorn_count desc) as rank_idx "
        "from public.player) as lb where high_acorn_count > 0 LIMIT 10");

    ret_response.done = &acorn_count_leaderboard;
  }

  if (PQntuples(params->player_pos) == 0)
  {
    PQclear(params->player_pos);
    error_message(event, "There aren't enough entries yet!");
    leaderboard_info_cleanup(params);
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

/*

general player leaderboard (rank <= 10)
---
select rank_idx, user_id, high_acorn_count
from (
select user_id,
high_acorn_count,
dense_rank() over (order by high_acorn_count desc) as rank_idx
from public.player) as lb where high_acorn_count > 0 LIMIT 10;

specific player leaderboard (rank > 10)
---
select rank_idx, user_id, high_acorn_count
from (
select user_id,
high_acorn_count,
dense_rank() over (order by high_acorn_count desc) as rank_idx
from public.player) as lb where user_id = %ld;



scurry leaderboard (rank <= 10)
---
select rank_idx, owner_id, name, total_stolen_acorns
from (
select owner_id, name,
SUM(stolen_acorns) as total_stolen_acorns,
dense_rank() over (order by SUM(stolen_acorns) desc) as rank_idx
from public.player join scurry on player.scurry_id = scurry.owner_id group by owner_id, name)
as lb where total_stolen_acorns > 0 LIMIT 10;

scurry leaderboard (rank > 10)
---
select rank_idx, owner_id, name, total_stolen_acorns
from (
select owner_id, name,
SUM(stolen_acorns) as total_stolen_acorns,
dense_rank() over (order by SUM(stolen_acorns) desc) as rank_idx
from public.player join scurry on player.scurry_id = scurry.owner_id group by owner_id, name)
as lb where owner_id = %ld;

*/