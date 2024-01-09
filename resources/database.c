PGconn* establish_connection(char* conninfo)
{
  PGconn* db_conn = PQconnectdb(conninfo);

  if (PQstatus(db_conn) != CONNECTION_OK)
  {
    fprintf(stderr, "%s", PQerrorMessage(db_conn) );
    PQfinish(db_conn);
    exit(1);
  }

  PQexec(db_conn, "SELECT pg_catalog.set_config('search_path', '', false)");

  return db_conn;
}

// LOADING FUNCTIONS

void load_game_struct(struct sd_pie_game *game, struct sd_player *player, unsigned long user_id)
{
  PGresult* search_game = (PGresult*) { 0 };
  search_game = SQL_query(search_game, 
      "select * from public.pies where user_id = %ld",
      user_id);

  // if starting a new game
  if (PQntuples(search_game) == 0)
  {
    PQclear(search_game);

    struct sd_pie_game new_game = { 0 };

    // generate a fresh new next piece
    int next_piece = rand() % PIECES_SIZE;
    generate_new_piece(&new_game.next_piece, &pieces[next_piece]);

    snprintf(new_game.history, sizeof(new_game.history), EMPTY_HISTORY, next_piece);
    
    shift_pieces(&new_game);

    search_game = SQL_query(search_game,
        "insert into public.pies values(%ld, 0, 1, '%s', '"EMPTY_PIE"', '"EMPTY_PIE"', '"EMPTY_PIE"', '%s', '%s') \n", 
        user_id, new_game.history, new_game.current_piece.encoded_buf, new_game.next_piece.encoded_buf);
  }
  PQclear(search_game);

  search_game = SQL_query(search_game, 
      "select * from public.pies where user_id = %ld",
      user_id);
  
  *game = (struct sd_pie_game)
  {
    .user_id = strtobigint( PQgetvalue(search_game, 0, GAME_USER_ID) ),
    .score = strtoint( PQgetvalue(search_game, 0, GAME_SCORE) ),
    .piece_count = strtoint( PQgetvalue(search_game, 0, GAME_PIECE_COUNT) )
  };

  player->biome = game->score/BIOME_INTERVAL % BIOME_SIZE;
  player->biome_num = game->score/BIOME_INTERVAL;

  // load history buf
  snprintf(game->history, sizeof(game->history), "%s", PQgetvalue(search_game, 0, GAME_HISTORY) );

  // load each encoded pie buf and then decode
  snprintf(game->pie_1.encoded_buf, sizeof(game->pie_1.encoded_buf), "%s", PQgetvalue(search_game, 0, GAME_PIE_ONE) );
  decode_pie(&game->pie_1);

  snprintf(game->pie_2.encoded_buf, sizeof(game->pie_2.encoded_buf), "%s", PQgetvalue(search_game, 0, GAME_PIE_TWO) );
  decode_pie(&game->pie_2);

  snprintf(game->pie_3.encoded_buf, sizeof(game->pie_3.encoded_buf), "%s", PQgetvalue(search_game, 0, GAME_PIE_THREE) );
  decode_pie(&game->pie_3);

  snprintf(game->current_piece.encoded_buf, sizeof(game->current_piece.encoded_buf), "%s", PQgetvalue(search_game, 0, GAME_CURRENT_PIECE) );
  decode_pie(&game->current_piece);

  snprintf(game->next_piece.encoded_buf, sizeof(game->next_piece.encoded_buf), "%s", PQgetvalue(search_game, 0, GAME_NEXT_PIECE) );
  decode_pie(&game->next_piece);
}

void load_player_struct(struct sd_player *player, unsigned long user_id, char* custom_id)
{  
  PGresult* search_player = (PGresult*) { 0 };
  search_player = SQL_query(search_player, 
      "select * from public.player where user_id = %ld",
      user_id);

  if (PQntuples(search_player) == 0)
  {
    PQclear(search_player);

    search_player = SQL_query(search_player,
        "insert into public.player values(%ld, 0, 0, 0, 0, 0, -1, 0, 0, 0, -1, 0, 4, 0, 0); \n"
        "insert into public.biome_story values(%ld, 0, 0, 0, 0, 0); \n", 
        user_id, user_id);
  }
  PQclear(search_player);

  search_player = SQL_query(search_player,
      "select * from public.player \n"
      "join public.biome_story on player.user_id = biome_story.user_id \n"
      "where player.user_id = %ld",
      user_id);

  *player = (struct sd_player)
  {
    .user_id = strtobigint( PQgetvalue(search_player, 0, DB_USER_ID) ),
    .scurry_id = strtobigint( PQgetvalue(search_player, 0, DB_SCURRY_ID) ),
    .color = strtoint( PQgetvalue(search_player, 0, DB_COLOR) ),
    .squirrel = strtoint( PQgetvalue(search_player, 0, DB_SQUIRREL) ),

    .high_acorn_count = strtoint( PQgetvalue(search_player, 0, DB_HIGH_ACORN_COUNT) ),
    .conjured_acorns = strtoint( PQgetvalue(search_player, 0, DB_CONJURED_ACORNS) ),
    .stolen_acorns = strtoint( PQgetvalue(search_player, 0, DB_STOLEN_ACORNS) ),
  
    .section = strtoint( PQgetvalue(search_player, 0 , DB_SECTION) ),
    .encounter = strtoint( PQgetvalue(search_player, 0, DB_ENCOUNTER) ),
    .timestamp = strtobigint( PQgetvalue(search_player, 0, DB_TIMESTAMP) ),
    .main_cd = strtobigint( PQgetvalue(search_player, 0, DB_MAIN_CD) ),
    .purchased = strtobigint( PQgetvalue(search_player, 0, DB_PURCHASED) ),
    .treasury_filled = strtoint( PQgetvalue(search_player, 0, DB_TREASURY) ),
    .pies_complete = strtoint( PQgetvalue(search_player, 0, DB_DAILY_PIES) ),
    .tm_mday = strtoint( PQgetvalue(search_player, 0, DB_DAILY_DAY) ),

    .story = {
      .grasslands = strtoint( PQgetvalue(search_player, 0, DB_GL_STORY) ),
      .seeping_sands = strtoint( PQgetvalue(search_player, 0, DB_SP_STORY) ),
      .nature_end = strtoint( PQgetvalue(search_player, 0, DB_NE_STORY) ),
      .death_grip = strtoint( PQgetvalue(search_player, 0, DB_DG_STORY) ),
      .last_acorn = strtoint( PQgetvalue(search_player, 0, DB_LA_STORY) )
    }
  };

  player->button_idx = (custom_id) ? custom_id[1] -48 : ERROR_STATUS;

  struct tm *info = get_UTC();
  if (info->tm_mday != player->tm_mday)
  {
    player->tm_mday = info->tm_mday;
    player->pies_complete = 0;
  }

  PQclear(search_player);
}

void load_scurry_struct(struct sd_scurry *scurry, unsigned long scurry_id)
{
  if (scurry_id > 0)
  {
    PGresult* scurry_db = (PGresult*) { 0 };
    scurry_db = SQL_query(scurry_db,
        "select * from public.scurry where owner_id = %ld", scurry_id);

    PGresult* members = (PGresult*) { 0 };
    members = SQL_query(members, "select sum(stolen_acorns) from public.player where scurry_id = %ld", scurry_id);

    *scurry = (struct sd_scurry) 
    {
      .scurry_owner_id = strtobigint( PQgetvalue(scurry_db, 0, DB_SCURRY_OWNER_ID) ),
      .war_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_STASH) ),
      .war_flag = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_FLAG) ),
      .total_stolen_acorns = strtoint( PQgetvalue(members, 0, 0) ),
      .prev_stolen_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_PREV_STOLEN_ACORNS) )
    };

    u_snprintf(scurry->scurry_name, sizeof(scurry->scurry_name), PQgetvalue(scurry_db, 0, DB_SCURRY_NAME));
    PQclear(scurry_db);

    scurry->rank = (scurry->total_stolen_acorns < SEED_NOT_MAX) ? SEED_NOT
        : (scurry->total_stolen_acorns < OAKFFICIAL_MAX) ? OAKFFICIAL : ROYAL_NUT;

    scurry->pie_count = scurry->rank +1;

    scurry->war_acorn_cap = scurry->pie_count * SLICES_SIZE;

    PQclear(members);
  }

}

// UPDATE FUNCTIONS

void update_game_row(struct sd_pie_game *game)
{
  char sql_str[1024] = { };

  // update encoded bufs for each player pie
  u_snprintf(sql_str, sizeof(sql_str),
      "update public.pies set "
        "piece_count = %d, "
        "score = %d, "
        "history = '%s', "
        "pie_one = '%s', "
        "pie_two = '%s', "
        "pie_three = '%s', "
        "current_piece = '%s', "
        "next_piece = '%s' "
      "where user_id = %ld; \n",
      game->piece_count, game->score, game->history,
      game->pie_1.encoded_buf, game->pie_2.encoded_buf, game->pie_3.encoded_buf,
      game->current_piece.encoded_buf, game->next_piece.encoded_buf,
      game->user_id);

  PGresult* player_update = (PGresult*) { 0 };
  player_update = SQL_query(player_update, sql_str);
  PQclear(player_update);
}

void update_player_row(struct sd_player *player)
{
  char sql_str[4096] = { };

  u_snprintf(sql_str, sizeof(sql_str),
      "BEGIN; \n"
      "update public.player set "
        "scurry_id = %ld, "
        "color = %d, "
        "squirrel = %d, "
        "high_acorn_count = %d, "
        "conjured_acorns = %d, "
        "stolen_acorns = %d, "
        "encounter = %d, "
        "section = %d, "
        "purchased = %d, "
        "treasury_filled = %d, "
        "pies_complete = %d, "
        "tm_mday = %d, "
        "t_stamp = %ld, "
        "main_cd = %ld \n"
      "where user_id = %ld; \n",
      player->scurry_id, player->color, player->squirrel, 
      player->high_acorn_count, player->conjured_acorns, 
      player->stolen_acorns, player->encounter, player->section, 
      player->purchased, player->treasury_filled, player->pies_complete,
      player->tm_mday, player->timestamp, player->main_cd,
      player->user_id);

  u_snprintf(sql_str, sizeof(sql_str),
      "update public.biome_story set "
        "grasslands = %d, "
        "seeping_sands = %d, "
        "nature_end = %d, "
        "death_grip = %d, "
        "last_acorn = %d "
      "where user_id = %ld; \n"
      "COMMIT;",
      player->story.grasslands, player->story.seeping_sands, player->story.nature_end,
      player->story.death_grip, player->story.last_acorn, 
      player->user_id);

  PGresult* player_update = (PGresult*) { 0 };
  player_update = SQL_query(player_update, sql_str);
  PQclear(player_update);
}

void update_scurry_row(struct sd_scurry *scurry)
{
  char sql_str[1028] = { };

  u_snprintf(sql_str, sizeof(sql_str),
      "update public.scurry set "
        "prev_stolen_acorns = %d, "
        "war_acorns = %d, "
        "war_flag = %d \n"
      "where owner_id = %ld \n",
      scurry->prev_stolen_acorns, scurry->war_acorns, scurry->war_flag,
      scurry->scurry_owner_id);
    
  PGresult* scurry_update = (PGresult*) { 0 };
  scurry_update = SQL_query(scurry_update, sql_str);
  PQclear(scurry_update);
}