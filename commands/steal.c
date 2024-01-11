struct sd_steal_info
{
  struct discord_component button;
  char custom_id[64];
  char label[64];

  struct discord_emoji emoji;
  char emoji_name[64];

  struct discord_embed_field fields[2]; // +1 offset for balance field
  char field_names[2][128];
  char field_values[2][512];

  char description[512];

  struct sd_player *player;

  int steal_amt;
  unsigned long target_user_id;
  int is_member;
  char username[64];
  char content[256];

  char footer_text[64];
  char footer_url[128];
};

void steal_info_cleanup(struct sd_steal_info *params)
{
  free(params->player);
  free(params);
}

void init_steal_fields(const struct discord_interaction *event, struct sd_steal_info *params, struct sd_player *player)
{
  APPLY_NUM_STR(conjured_acorns, player->conjured_acorns);

  params->fields[0] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Balance"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        ""INDENT" "CONJURED_ACORNS" Conjured Acorns: **%s**", conjured_acorns)
  };

  struct sd_pie treasury = (struct sd_pie) { 0 };

  int piece_idx = 0;
  for (;piece_idx < player->treasury_filled; piece_idx++)
    treasury.decoded_buf[piece_idx] = TYPE_GOLDEN_ACORNS;

  for (; piece_idx < PIECES_SIZE; piece_idx++)
    treasury.decoded_buf[piece_idx] = TYPE_EMPTY;

  params->fields[1] = (struct discord_embed_field) {
    .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Treasury"),
    .value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), format_pie(&treasury))
  };

  if (params->steal_amt > 0)
    u_snprintf(params->field_values[1], sizeof(params->field_values[1]), "+**%d** "GOLDEN_ACORNS"", params->steal_amt);

  if (event->data->custom_id[0] != TYPE_INIT_STEAL)
    u_snprintf(params->field_values[1], sizeof(params->field_values[1]), " "INDENT" -**%d** "CONJURED_ACORNS" ", STEAL_COST);
}

void init_steal_buttons(struct sd_steal_info *params, struct sd_player *player)
{
  params->emoji = (struct discord_emoji) {
    .name = u_snprintf(params->emoji_name, sizeof(params->emoji_name), slice_types[TYPE_CONJURED_ACORNS].item.emoji_name),
    .id = slice_types[TYPE_CONJURED_ACORNS].item.emoji_id
  };
  params->button = (struct discord_component) {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = u_snprintf(params->label, sizeof(params->label), "(-%d) Steal", STEAL_COST),
    .custom_id = u_snprintf(params->custom_id, sizeof(params->custom_id), 
        "%c0.%ld", TYPE_STEAL, player->user_id),
    .emoji = &params->emoji
  };

  if (player->conjured_acorns >= STEAL_COST)
  {
    params->button.style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    params->button.style = DISCORD_BUTTON_SECONDARY;
    params->button.disabled = true;
  }
}

void steal_cmd_state(const struct discord_interaction *event, struct sd_steal_info *params)
{
  if (event->data->custom_id[0] == TYPE_INIT_STEAL)
  {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to your Treasury!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, misc[MISC_ALERT].file_path);
  }
  else {
    struct sd_player *player = params->player;

    int chance = rand() % MAX_CHANCE;

    if (chance < STEAL_CHANCE)
    {
      if (params->is_member == 1)
        discord_create_message(client, event->channel_id, 
            &(struct discord_create_message)
            {
              .content = u_snprintf(params->content, sizeof(params->content), 
                  "**%s**, **%s** failed to snatch from your treasury!", 
                  params->username, event->member->user->username)
            },
            NULL);
      params->steal_amt = 0;
    }
    else {
      player->treasury_filled += params->steal_amt;

      PGresult* target = (PGresult*) { 0 };
      target = SQL_query(target, "update public.player set treasury_filled = treasury_filled - %d where user_id = %ld",
          params->steal_amt, params->target_user_id);
      PQclear(target);
    }

    player->conjured_acorns -= STEAL_COST;
  }
}

void complete_treasury(const struct discord_interaction *event, struct sd_steal_info *params)
{  
  struct sd_player *player = params->player;

  steal_cmd_state(event, params);

  init_steal_fields(event, params, player);
  init_steal_buttons(params, player);

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player->color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Treasury"),
    .description = u_snprintf(params->description, sizeof(params->description), 
        " "OFF_ARROW" Steal golden acorns from other players' treasuries to fill your own for bonus stash completion rewards! \n"
        " "OFF_ARROW" When there are more than **4** golden acorns present, your stash completions are doubled!"),
    
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, 
          squirrels[player->squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params->fields,
      .size = 2
    },
    .footer = &(struct discord_embed_footer) {
      .text = params->footer_text,
      .icon_url = params->footer_url
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = &params->button,
        .size = 1
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

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_UPDATE_MESSAGE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = action_rows,
        .size = 2
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(player);

  steal_info_cleanup(params);
}

/* if user, format string using backticks */
void steal_from_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  struct sd_steal_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  u_snprintf(params->username, sizeof(params->username), "`%s`", user->username);

  complete_treasury(event, params);
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

  discord_get_user(client, params->target_user_id, &ret_user);
}

/* if member, format string using mention */
void steal_from_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  (void)client;
  struct sd_steal_info *params = resp->data;
  const struct discord_interaction *event = resp->keep;

  params->is_member = 1;

  u_snprintf(params->username, sizeof(params->username), "<@%ld>", member->user->id);

  complete_treasury(event, params);
}

int steal_interaction(const struct discord_interaction *event)
{
  struct sd_steal_info *params = calloc(1, sizeof(struct sd_steal_info));

  params->player = calloc(1, sizeof(struct sd_player));
  struct sd_player *player = params->player;
  load_player_struct(player, event->member->user->id, event->data->custom_id);

  if (event->message->timestamp /1000 < player->timestamp)
  {
    error_message(event, "This appears to be an old session! Please renew your session by sending `/start`.");
    steal_info_cleanup(params);
    return ERROR_STATUS;
  }

  player->timestamp = event->message->timestamp /1000;

  // if steal embed is being initialized, just skip right to completing the interaction
  if (event->data->custom_id[0] == TYPE_INIT_STEAL)
  {
    complete_treasury(event, params);
    return 0;
  }

  PGresult* search_player = (PGresult*) { 0 };

  params->steal_amt = (player->treasury_filled <= 6 && rand() % MAX_CHANCE > DOUBLE_STEAL) ? 2 : 1;

  if (player->scurry_id > 0)
  {
    search_player = SQL_query(search_player, 
        "select * from public.player where treasury_filled >= %d and user_id != %ld and scurry_id != %ld order by random() LIMIT 1;", 
        params->steal_amt, player->user_id, player->scurry_id);
  }
  else {
    search_player = SQL_query(search_player, 
        "select * from public.player where treasury_filled >= %d and user_id != %ld order by random() LIMIT 1;",
        params->steal_amt, player->user_id);
  }

  if (PQntuples(search_player) == 0)
  {
    error_message(event, "Sorry, there is no available treasury to loot right now!");
    PQclear(search_player);
    steal_info_cleanup(params);

    return ERROR_STATUS;
  }

  params->target_user_id = strtobigint(PQgetvalue(search_player, 0, DB_USER_ID) );

  struct discord_ret_guild_member ret_member = {
    .done = &steal_from_member,
    .fail = &steal_not_member,
    .data = params,
    .keep = event
  };

  // check if guild member
  discord_get_guild_member(client, event->guild_id, params->target_user_id, &ret_member);

  PQclear(search_player);

  return 0;
}
