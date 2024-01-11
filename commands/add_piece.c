struct sd_add_piece
{
  struct discord_component buttons[MAX_PIES +1]; // +1 for rotation
  char custom_ids[MAX_PIES +1][64];
  char labels[MAX_PIES +1][64];

  struct discord_emoji emoji;
  char emoji_name[64];

  struct discord_embed_field fields[MAX_PIES +3 +1];
  char field_names[MAX_PIES +3 +1][64]; // +4 for ongoing score, current, next, and a space!
  char field_values[MAX_PIES +3 +1][1024];

  char description[1024];
  char image_url[128];

  char footer_text[64];
  char footer_url[128];

  int score;
  int conjured_acorns;
  int war_acorns;
  int daily_reward;
};

void complete_encounter(struct sd_add_piece *params, struct sd_player *player)
{
  int encounter_idx = 0;
  for (int i = 1; i < player->section +1; i++)
  {
    // count all whole sections
    encounter_idx += biomes[player->biome].sections[player->section - i].section_size;
  }

  int* biome_stories[5] = { 
    &player->story.grasslands, 
    &player->story.seeping_sands, 
    &player->story.nature_end, 
    &player->story.death_grip, 
    &player->story.last_acorn 
  };

  // add progress through current section
  encounter_idx += player->encounter;

  // if encounter idx bit is on or player didn't chance info, this encounter has been seen!
  if ((*biome_stories[player->biome] >> encounter_idx & 0x01) == 1 || rand() % MAX_CHANCE < STORY_CHANCE)
    return;

  // now assume the encounter hasn't been seen!

  // switch encounter idx bit on
  (*biome_stories[player->biome]) |= (1 << encounter_idx);

  u_snprintf(params->description, sizeof(params->description), "\nNew element of the story unlocked! \n"QUEST_MARKER" \"*%s*\" \n", 
      biomes[player->biome].sections[player->section].encounters[player->encounter].context);
}

void init_game_fields(struct sd_add_piece *params, struct sd_player *player, struct sd_pie_game *game)
{
  char* labels[MAX_PIES] = { "I", "II", "III" };
  struct sd_pie *pies[3] = { &game->pie_1, &game->pie_2, &game->pie_3 };

  APPLY_NUM_STR(score, game->score);
  APPLY_NUM_STR(req_acorn_count, (player->biome_num +1) * BIOME_INTERVAL);
  params->fields[0] = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Score"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        " "INDENT" "ACORN_COUNT" Acorn Count: **%s**",
        score)
  };

  if (player->high_acorn_count > 0)
  {
    APPLY_NUM_STR(high_acorn_count, player->high_acorn_count);
    u_snprintf(params->field_values[0], sizeof(params->field_values[0]), " ("LEADER"**%s**)", high_acorn_count);
  }

  u_snprintf(params->field_values[0], sizeof(params->field_values[0]), "\n "INDENT" "ACORNS" Piece #**%d** \n", game->piece_count);

  // acorn count is updated before biome_num and can be used to predict when the biome will change at this stage
  if (game->score/BIOME_INTERVAL > player->biome_num)
  {
    // POSSIBILITY of skipping biomes
    struct sd_file_data next_biome_data = biomes[(game->score/BIOME_INTERVAL) % BIOME_SIZE].biome_icon;
    u_snprintf(params->field_values[0], sizeof(params->field_values[0]), "\n "QUEST_MARKER" You have reached <:%s:%ld> **%s**!\n",
        next_biome_data.emoji_name, next_biome_data.emoji_id, next_biome_data.formal_name);
  }

  params->fields[1] = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "__Current__"),
    .value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), format_pie(&game->current_piece) ),
    .Inline = true
  };

  if (player->encounter > ERROR_STATUS)
    u_snprintf(params->field_names[1], sizeof(params->field_names[1]), " ("QUEST_MARKER")");

  params->fields[2] = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_names[2], sizeof(params->field_names[2]), " "),
    .value = u_snprintf(params->field_values[2], sizeof(params->field_values[2]), " "),
    .Inline = true
  };

  params->fields[3] = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_names[3], sizeof(params->field_names[3]), "__Next__"),
    .value = u_snprintf(params->field_values[3], sizeof(params->field_values[3]), format_pie(&game->next_piece) ),
    .Inline = true
  };

  for (int pie_idx = 0; pie_idx < MAX_PIES; pie_idx++)
  {
    int field_idx = pie_idx +4;

    params->fields[field_idx] = (struct discord_embed_field)
    {
      .name = u_snprintf(params->field_names[field_idx], sizeof(params->field_names[field_idx]), " "TREASURE" %s", labels[pie_idx]),
      .value = u_snprintf(params->field_values[field_idx], sizeof(params->field_values[field_idx]), format_pie(pies[pie_idx]) ),
      .Inline = true
    };
  }

  // update scores below pie!
  if (params->score > 0)
  {
    APPLY_NUM_STR(score, params->score);
    u_snprintf(params->field_values[player->button_idx +4], sizeof(params->field_values[player->button_idx +4]), 
        "+**%s** "ACORN_COUNT"", score);

    if (params->conjured_acorns > 0)
      u_snprintf(params->field_values[player->button_idx +4], sizeof(params->field_values[player->button_idx +4]), 
        "\n+**%d** "CONJURED_ACORNS"", params->conjured_acorns);

    if (params->war_acorns > 0)
      u_snprintf(params->field_values[player->button_idx +4], sizeof(params->field_values[player->button_idx +4]), 
        "\n+**%d** "WAR_ACORNS"", params->war_acorns);
    
    if (params->daily_reward > 0)
      u_snprintf(params->field_values[0], sizeof(params->field_values[0]), 
        "\n"STAHR" Milestone complete!"
        "\n+**10** "CONJURED_ACORNS"");
  }
}

void init_game_buttons(struct sd_add_piece *params, struct sd_pie_game *game)
{
  char* labels[MAX_PIES] = { "I", "II", "III" };
  struct sd_pie *pies[3] = { &game->pie_1, &game->pie_2, &game->pie_3 };

  params->emoji = (struct discord_emoji)
  {
    .name = u_snprintf(params->emoji_name, sizeof(params->emoji_name), misc[MISC_TREASURE].emoji_name),
    .id = misc[MISC_TREASURE].emoji_id
  };

  for (int button_idx = 0; button_idx < MAX_PIES; button_idx++)
  {
    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), labels[button_idx]),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d.%ld",
                    TYPE_ADD_PIECE, button_idx, game->user_id),
      .emoji = &params->emoji
    };

    if (check_piece_fit(pies[button_idx], &game->current_piece) == 0)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
    }
  }

  if (check_end(game) == 1)
  {
    params->buttons[MAX_PIES] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_DANGER,
      .custom_id = u_snprintf(params->custom_ids[MAX_PIES], sizeof(params->custom_ids[MAX_PIES]), "%c%d.%ld",
          TYPE_ADD_PIECE, DETECT_RESTART, game->user_id),
      .label = "restart"
    };
  }
  else {
    params->buttons[MAX_PIES] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_SUCCESS,
      .custom_id = u_snprintf(params->custom_ids[MAX_PIES], sizeof(params->custom_ids[MAX_PIES]), "%c%d.%ld",
          TYPE_ADD_PIECE, DETECT_ROTATE, game->user_id),
      .label = "rotate"
    };
  }
}

void generate_rewards(struct sd_pie *pie, struct sd_add_piece *params, struct sd_player *player, struct sd_scurry *scurry, struct sd_pie_game *game)
{
  player->pies_complete++;

  if (player->pies_complete == FIRST_DAILY
    || player->pies_complete == SECOND_DAILY
    || player->pies_complete == THIRD_DAILY)
  {
    params->daily_reward = DAILY_REWARD;
    player->conjured_acorns += DAILY_REWARD;
  }

  params->conjured_acorns = check_types(pie, TYPE_CONJURED_ACORNS);

  if (params->conjured_acorns > 0)
  {
    player->conjured_acorns += params->conjured_acorns;
    params->score *= 2 * params->conjured_acorns;
  }

  // only try stealing if chanced and scurry is at war
  if (rand() % MAX_CHANCE > EVENT_CHANCE && scurry->war_flag == 1)
  {
    // must have enough space
    int steal_amt = (scurry->war_acorns <= scurry->war_acorn_cap -2 && rand() % MAX_CHANCE > DOUBLE_STEAL) ? 2 : 1;

    // must be an available opponent
    PGresult* search_scurry = (PGresult*) { 0 };
    search_scurry = SQL_query(search_scurry, "select * from public.scurry where war_acorns >= %d and owner_id != %ld order by random() LIMIT 1", 
        steal_amt, scurry->scurry_owner_id);

    if (PQntuples(search_scurry) > 0)
    {
      struct sd_scurry opponent = { 0 };
      load_scurry_struct(&opponent, strtobigint( PQgetvalue(search_scurry, 0, DB_SCURRY_OWNER_ID) ) );

      params->war_acorns = steal_amt;

      player->stolen_acorns += params->war_acorns;
      // dont overfill scurry stash!
      scurry->war_acorns = (scurry->war_acorns + params->war_acorns > scurry->war_acorn_cap) 
          ? scurry->war_acorn_cap : scurry->war_acorns + params->war_acorns;

      opponent.war_acorns -= steal_amt;
      if (opponent.war_acorns == 0)
      {
        opponent.war_flag = 0;
        opponent.war_acorns = opponent.war_acorn_cap/2;
      }

      update_scurry_row(&opponent);
    }
  }

  if (player->treasury_filled > 4)
    params->score *= 1.5;

  // buff only lasts for duration of war when war acorns > half cap!
  if (scurry->war_acorns > scurry->war_acorn_cap/2)
    params->score *= SCURRY_RANK_BONUS * (scurry->rank +1);

  game->score += params->score;

  *pie = (struct sd_pie) { 0 };
}

void game_cmd_state(struct sd_add_piece *params, struct sd_player *player, struct sd_scurry *scurry, struct sd_pie_game *game)
{  
  switch (player->button_idx)
  {
    case DETECT_START:
      u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to Squirrel Dash!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, squirrels[GRAY_SQUIRREL].squirrel.file_path);
      break;
    case DETECT_ROTATE:
      rotate_pie(&game->current_piece);
      break;
    case DETECT_RESTART:
      // add high score before deletion!
      if (game->score > player->high_acorn_count)
        player->high_acorn_count = game->score;

      // make sure encounter is reset even if it already is!
      player->encounter = ERROR_STATUS;
      player->section = ERROR_STATUS;

      PGresult* game_update = (PGresult*) { 0 };
      game_update = SQL_query(game_update, "delete from public.pies where user_id = %ld \n", game->user_id);
      PQclear(game_update);

      // renew session
      load_game_struct(game, player, player->user_id);
      break;
    default:
      // it is assumed at this point that current piece will fit the selected pie!

      struct sd_pie *pies[3] = { &game->pie_1, &game->pie_2, &game->pie_3 };

      merge_pies(pies[player->button_idx], &game->current_piece);

      params->score = check_complete_pie(pies[player->button_idx]);

      if (params->score > 0)
        generate_rewards(pies[player->button_idx], params, player, scurry, game);

      shift_pieces(game);

      // try an encounter
      if (player->encounter == ERROR_STATUS
        && rand() % MAX_CHANCE < ENCOUNTER_CHANCE)
      {
        player->section = rand() % biomes[player->biome].biome_size;
        player->encounter = rand() % biomes[player->biome].sections[player->section].section_size;

        int new_piece = rand() % PIECES_SIZE;

        // pick a new piece that hasnt been used yet  
        while (search_history(game->history, new_piece) == 1)
          new_piece = rand() % PIECES_SIZE;

        // add next piece to history
        game->history[(game->piece_count +1) % 5] = (char)(new_piece +48);

        generate_new_piece(&game->current_piece, &pieces[new_piece]);
      }
      else if (player->encounter != ERROR_STATUS)
      {        
        complete_encounter(params, player);
        player->encounter = ERROR_STATUS;
        player->section = ERROR_STATUS;
      }
  }

}


int add_piece_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  ERROR_INTERACTION((event->message->timestamp /1000 < player.timestamp), 
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp /1000;

  struct sd_add_piece params = { 0 };

  struct sd_pie_game game = { 0 };
  load_game_struct(&game, &player, player.user_id);

  struct sd_scurry scurry = { 0 };
  load_scurry_struct(&scurry, player.scurry_id);

  struct sd_header_params header = { 0 };

  game_cmd_state(&params, &player, &scurry, &game);

  init_game_fields(&params, &player, &game);

  init_game_buttons(&params, &game);

  if (player.encounter == ERROR_STATUS)
  {
    u_snprintf(header.title, sizeof(header.title), "Foraging");
    u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, 
        squirrels[player.squirrel].squirrel.file_path);
  }
  else if (player.encounter > ERROR_STATUS) 
  {
    struct sd_encounter encounter = biomes[player.biome].sections[player.section].encounters[player.encounter];

    u_snprintf(header.title, sizeof(header.title), encounter.name);
    u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
        encounter.file_path);
    u_snprintf(params.description, sizeof(params.description), encounter.conflict);
  }

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
        .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
        .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
            "https://cdn.discordapp.com/avatars/%lu/%s.png",
            event->member->user->id, event->member->user->avatar)
    },
    .title = header.title,
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = header.thumbnail_url
    },
    .description = params.description,
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = MAX_PIES +3 +1
    },
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player.biome].biome_scene_path)
    },
    .footer = &(struct discord_embed_footer)
    {
      .text = params.footer_text,
      .icon_url = params.footer_url
    }
  };

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = MAX_PIES +1
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

  update_game_row(&game);
  update_player_row(&player);
  update_scurry_row(&scurry);

  return 0;
}