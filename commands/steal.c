
struct sd_steal_info {
  char username[64];

  struct sd_player *player;

  struct discord_component button;
  char custom_id[128];
  char label[64];

  struct discord_emoji emoji;
  char emoji_name[64];

  char content[256];
  char description[512];

  char footer_text[64];
  char footer_url[128];

  unsigned long t_user_id;
  int is_member;
  int steal_amt;
  int golden_acorns;
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

    if (params->is_member == 1)
      discord_create_message(client, event->channel_id, 
          &(struct discord_create_message)
          {
            .content = u_snprintf(params->content, sizeof(params->content), 
                "**%s**, **%s** failed to snatch **%s** "ACORNS" acorns!", 
                params->username, event->member->user->username, steal_amt)
          },
          NULL);
    
    u_snprintf(params->description, sizeof(params->description),
        "%s caught you trying to steal **%s** "ACORNS" acorns! \n"
        "\n-**20** "ENERGY" Energy \n", 
        params->username, steal_amt);
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

    daily_task_progression(event, player, TASK_SEEDY);

    APPLY_NUM_STR(golden_acorns, params->golden_acorns);

    u_snprintf(params->description, sizeof(params->description),
        "You anonymously stole **%s** "ACORNS" acorns! \n"
        "\n+**%s** "GOLDEN_ACORNS" Golden Acorns \n"
        "\n-**20** "ENERGY" Energy \n",
        steal_amt, golden_acorns);
  }
  
  player->energy -= STEAL_ENERGY_COST;

  header.embed.color = color;

  header.embed.author = &(struct discord_embed_author) 
  {
    .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
    .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
        "https://cdn.discordapp.com/avatars/%lu/%s.png",
        event->member->user->id, event->member->user->avatar)
  };

  header.embed.title = header.title;

  header.embed.thumbnail = &(struct discord_embed_thumbnail) {
    .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
        (player->designer_squirrel == ERROR_STATUS) 
          ? squirrels[player->squirrel].squirrel.file_path 
          : designer_squirrels[player->designer_squirrel].squirrel.file_path)
  };
  
  header.embed.description = params->description;

  header.embed.footer = &(struct discord_embed_footer) 
  {
    .text = u_snprintf(params->footer_text, sizeof(params->footer_text), 
        "You have %d energy left!", player->energy),
    .icon_url = u_snprintf(params->footer_url, sizeof(params->footer_url), 
        GIT_PATH, items[ITEM_ENERGY].file_path)
  };

  player->main_cd = time(NULL) + COOLDOWN;

  params->emoji = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_name, sizeof(params->emoji_name), item_types[TYPE_ACORN_SACK].emoji_name),
    .id = item_types[TYPE_ACORN_SACK].emoji_id
  };

  params->button = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = u_snprintf(params->custom_id, sizeof(params->custom_id), "%c.%ld",
        TYPE_STEAL, event->member->user->id),
    .label = u_snprintf(params->label, sizeof(params->label), "Steal!"),
    .emoji = &params->emoji
  };

  if (player->energy >= STEAL_ENERGY_COST)
  {
    params->button.style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    params->button.style = DISCORD_BUTTON_SECONDARY;
    params->button.disabled = true;
  }

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
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
      }
    }
  };

  if (player->daily.claim_primary 
    || player->daily.claim_secondary 
    || player->daily.claim_tertiary)
  {
    discord_edit_message(client, event->channel_id, event->message->id,
      &(struct discord_edit_message)
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
      },
      NULL);
  }
  else {
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

    char values[16384];
    discord_interaction_response_to_json(values, sizeof(values), &interaction);
    fprintf(stderr, "%s \n", values);

    discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
  }

  update_player_row(player);

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

  params->is_member = 1;

  u_snprintf(params->username, sizeof(params->username), "<@%ld>", member->user->id);

  steal_acorns(event, params);
}

int steal_interaction(const struct discord_interaction *event) 
{
  struct sd_player *player = calloc(1, sizeof(struct sd_player));
  load_player_struct(player, event->member->user->id, event->data->custom_id);

  energy_regen(player);
  ERROR_INTERACTION((time(NULL) < player->main_cd), "Cooldown not ready!");
  ERROR_INTERACTION((player->energy < STEAL_ENERGY_COST), "You need more energy!");

  float random_percent = (float)genrand(15, 5) /100;

  int steal_min = generate_price(player->stats.proficiency_lv, PROFICIENCY_UNIT);

  int relative_golden_acorns = BIOME_ENCOUNTER_COST * (player->biome_num +1);
  
  // select all players that isnt the player, game owner, within a range of acorns, or is the same scurry
  PGresult* t_user = (PGresult*) { 0 };
  
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
  params->golden_acorns = relative_golden_acorns;

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

struct sd_init_steal
{
  struct discord_component button;
  char custom_id[128];
  char label[64];

  struct discord_emoji emoji;
  char emoji_name[64];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

int init_steal_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_header_params header = { 0 };
  struct sd_init_steal params = { 0 };

  struct sd_file_data squirrel_dir = (player.designer_squirrel == ERROR_STATUS) 
      ? squirrels[player.squirrel].squirrel
      : designer_squirrels[player.designer_squirrel].squirrel;

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Stealing"),
    .description = u_snprintf(params.description, sizeof(params.description),
        " "OFF_ARROW" Stealing costs **20** "ENERGY" energy and isn't always successful. \n"
        " "OFF_ARROW" If the player stolen from is in the same server, they are notified! \n"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, squirrel_dir.file_path)
    },
    .footer = &(struct discord_embed_footer) 
    {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "\n Happy stealing! \n"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, squirrels[SQUIRREL_BOOKIE].squirrel.file_path)
    }
  };

  params.emoji = (struct discord_emoji)
  {
    .name = u_snprintf(params.emoji_name, sizeof(params.emoji_name), item_types[TYPE_ACORN_SACK].emoji_name),
    .id = item_types[TYPE_ACORN_SACK].emoji_id
  };

  params.button = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = u_snprintf(params.custom_id, sizeof(params.custom_id), "%c.%ld",
        TYPE_STEAL, event->member->user->id),
    .label = u_snprintf(params.label, sizeof(params.label), "Steal!"),
    .emoji = &params.emoji
  };

  if (player.energy >= STEAL_ENERGY_COST)
  {
    params.button.style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    params.button.style = DISCORD_BUTTON_SECONDARY;
    params.button.disabled = true;
  }

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = &params.button,
        .size = 1
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
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

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}