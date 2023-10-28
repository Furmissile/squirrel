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

// void load_player_struct(struct sd_player *player_res, unsigned long user_id)
void load_player_struct(struct sd_player *player_res, const struct discord_interaction *event)
{
  unsigned long user_id = (event->member) ? event->member->user->id : event->user->id;

  PGresult* search_player = (PGresult*) { 0 };
  search_player = SQL_query(search_player, 
      "select * from public.player where user_id = %ld",
      user_id);

  if (PQntuples(search_player) == 0)
  {
    PQclear(search_player);
    search_player = SQL_query(search_player,
        "BEGIN; \n"
        "insert into public.player values(%ld, 0, 0, 0, 100, 10, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1); \n"
        "insert into public.stats values(%ld, 1, 1, 1); \n"
        "insert into public.buffs values(%ld, 0, 0, 0); \n"
        "insert into public.biome_story values(%ld, 0, 0, 0, 0, 0); \n"
        "insert into public.session_data values(%ld, %ld, 0, 0, 0, 0, 0, 0, 0, 0, 0); \n"
        "COMMIT;", 
        user_id, user_id, user_id, user_id, user_id, time(NULL));
  }

  PQclear(search_player);

  search_player = SQL_query(search_player,
      "select * from public.player \n"
      "join public.stats on player.user_id = stats.user_id \n"
      "join public.buffs on player.user_id = buffs.user_id \n"
      "join public.biome_story on player.user_id = biome_story.user_id \n"
      "join public.session_data on player.user_id = session_data.user_id \n"
      "where player.user_id = %ld",
      user_id);

  *player_res = (struct sd_player)
  {
    .user_id = strtobigint( PQgetvalue(search_player, 0, DB_USER_ID) ),
    .scurry_id = strtobigint( PQgetvalue(search_player, 0, DB_SCURRY_ID) ),
    .color = strtoint( PQgetvalue(search_player, 0, DB_COLOR) ),
    .squirrel = strtoint( PQgetvalue(search_player, 0, DB_SQUIRREL) ),

    .energy = strtoint( PQgetvalue(search_player, 0, DB_ENERGY) ),
    .health = strtoint( PQgetvalue(search_player, 0, DB_HEALTH) ),

    .acorns = strtoint( PQgetvalue(search_player, 0, DB_ACORNS) ),
    .acorn_count = strtoint( PQgetvalue(search_player, 0, DB_ACORN_COUNT) ),
    .high_acorn_count = strtoint( PQgetvalue(search_player, 0, DB_HIGH_ACORN_COUNT) ),
    .golden_acorns = strtoint( PQgetvalue(search_player, 0, DB_GOLDEN_ACORNS) ),
    .spent_golden_acorns = strtoint( PQgetvalue(search_player, 0, DB_SPENT_GOLDEN_ACORNS) ),
    .conjured_acorns = strtoint( PQgetvalue(search_player, 0, DB_CONJURED_ACORNS)),
    .stolen_acorns = strtoint( PQgetvalue(search_player, 0, DB_STOLEN_ACORNS) ),
    .catnip = strtoint( PQgetvalue(search_player, 0, DB_CATNIP) ),
  
    .section = strtoint( PQgetvalue(search_player, 0 , DB_SECTION) ),
    .encounter = strtoint( PQgetvalue(search_player, 0, DB_ENCOUNTER) ),
    .main_cd = strtobigint( PQgetvalue(search_player, 0, DB_MAIN_CD) ),
    .purchased = strtobigint( PQgetvalue(search_player, 0, DB_PURCHASED) ),
    .designer_squirrel = strtobigint( PQgetvalue(search_player, 0, DB_DESIGNER_SQUIRREL) ),

    .stats = {
      .proficiency_lv = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_LV) ),
      .strength_lv = strtoint( PQgetvalue(search_player, 0, DB_STRENGTH_LV) ),
      .luck_lv = strtoint( PQgetvalue(search_player, 0, DB_LUCK_LV) )
    },

    .buffs = {
      .proficiency_acorn = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_ACORN) ),
      .luck_acorn = strtoint( PQgetvalue(search_player, 0, DB_LUCK_ACORN) ),
      .boosted_acorn = strtoint( PQgetvalue(search_player, 0, DB_BOOSTED_ACORN) )
    },

    .story = {
      .grasslands = strtoint( PQgetvalue(search_player, 0, DB_GL_STORY) ),
      .seeping_sands = strtoint( PQgetvalue(search_player, 0, DB_SP_STORY) ),
      .nature_end = strtoint( PQgetvalue(search_player, 0, DB_NE_STORY) ),
      .death_grip = strtoint( PQgetvalue(search_player, 0, DB_DG_STORY) ),
      .last_acorn = strtoint( PQgetvalue(search_player, 0, DB_LA_STORY) )
    },

    .session_data = {
      .start_time = strtobigint( PQgetvalue(search_player, 0, DB_SESSION_START_TIME) ),
      .acorn_count = strtoint( PQgetvalue(search_player, 0, DB_SESSION_ACORN_COUNT) ),
      .acorns = strtoint( PQgetvalue(search_player, 0, DB_SESSION_ACORNS) ),
      .golden_acorns = strtoint( PQgetvalue(search_player, 0, DB_SESSION_GOLDEN_ACORNS) ),
      .health_loss = strtoint( PQgetvalue(search_player, 0, DB_SESSION_HEALTH_LOSS) ),

      .no_acorns = strtoint( PQgetvalue(search_player, 0, DB_SESSION_NO_ACORNS) ),
      .acorn_handful = strtoint( PQgetvalue(search_player, 0, DB_SESSION_ACORN_HANDFUL) ),
      .acorn_mouthful = strtoint( PQgetvalue(search_player, 0, DB_SESSION_ACORN_MOUTHFUL) ),
      .lost_stash = strtoint( PQgetvalue(search_player, 0, DB_SESSION_LOST_STASH) ),
      .acorn_sack = strtoint( PQgetvalue(search_player, 0, DB_SESSION_ACORN_SACK) )
    }
  };

  player_res->biome = player_res->acorn_count/BIOME_INTERVAL % BIOME_SIZE;
  player_res->biome_num = player_res->acorn_count/BIOME_INTERVAL;
  player_res->max_health = generate_factor(player_res->stats.strength_lv, STRENGTH_FACTOR) + MAX_HEALTH;

  struct sd_session_data *data = &player_res->session_data;
  player_res->session_data.total_forages = data->no_acorns + data->acorn_handful + data->acorn_mouthful + data->lost_stash + data->acorn_sack;

  PQclear(search_player);
}

void load_scurry_struct(struct sd_scurry *scurry_res, unsigned long scurry_id)
{
  if (scurry_id > 0)
  {
    PGresult* scurry_db = (PGresult*) { 0 };
    scurry_db = SQL_query(scurry_db,
        "select * from public.scurry where owner_id = %ld", scurry_id);

    *scurry_res = (struct sd_scurry) 
    {
      .scurry_owner_id = strtobigint( PQgetvalue(scurry_db, 0, DB_SCURRY_OWNER_ID) ),
      .total_stolen_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_TOTAL_STOLEN_ACORNS) ),
      .war_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_STASH) ),
      .war_flag = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_FLAG) ),
      .prev_stolen_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_PREV_STOLEN_ACORNS) )
    };

    u_snprintf(scurry_res->scurry_name, sizeof(scurry_res->scurry_name), PQgetvalue(scurry_db, 0, DB_SCURRY_NAME));
    PQclear(scurry_db);

    PGresult* members = (PGresult*) { 0 };
    members = SQL_query(members, "select * from public.player where scurry_id = %ld", scurry_id);

    int best_acorn = (scurry_res->total_stolen_acorns > scurry_res->prev_stolen_acorns) 
        ? scurry_res->total_stolen_acorns : scurry_res->prev_stolen_acorns;

    scurry_res->rank = (best_acorn < SEED_NOT_MAX) ? SEED_NOT
        : (best_acorn < ACORN_SNATCHER_MAX) ? ACORN_SNATCHER
        : (best_acorn < SEED_SNIFFER_MAX) ? SEED_SNIFFER
        : (best_acorn < OAKFFICIAL_MAX) ? OAKFFICIAL : ROYAL_NUT;

    // default to 1000
    scurry_res->war_acorn_cap = (PQntuples(members) < SCURRY_MEMBER_REQ) 
        ? DEFAULT_WAR_STASH : PQntuples(members) * (WAR_STASH_FACTOR * (scurry_res->rank +1));
    
    PQclear(members);
  }

}

void update_player_row(struct sd_player *player_res)
{
  char sql_str[4096] = { };

  u_snprintf(sql_str, sizeof(sql_str),
      "BEGIN; \n"
      "update public.player set "
        "scurry_id = %ld, "
        "color = %d, "
        "squirrel = %d, "
        "energy = %d, "
        "health = %d, "
        "acorns = %d, "
        "acorn_count = %d, "
        "high_acorn_count = %d, "
        "golden_acorns = %d, "
        "spent_golden_acorns = %d, "
        "conjured_acorns = %d, "
        "stolen_acorns = %d, "
        "catnip = %d, "
        "encounter = %d, "
        "section = %d, "
        "purchased = %d, "
        "designer_squirrel = %d, "
        "main_cd = %ld \n"
      "where user_id = %ld; \n",
      player_res->scurry_id, player_res->color, player_res->squirrel, player_res->energy, 
      player_res->health,player_res->acorns, player_res->acorn_count, player_res->high_acorn_count, 
      player_res->golden_acorns, player_res->spent_golden_acorns, player_res->conjured_acorns, player_res->stolen_acorns, 
      player_res->catnip, player_res->encounter, player_res->section, player_res->purchased, 
      player_res->designer_squirrel, player_res->main_cd,
      player_res->user_id);
    
  u_snprintf(sql_str, sizeof(sql_str),
      "update public.stats set "
        "proficiency_lv = %d, "
        "strength_lv = %d, "
        "luck_lv = %d \n"
      "where user_id = %ld; \n",
      player_res->stats.proficiency_lv, player_res->stats.strength_lv, player_res->stats.luck_lv,
      player_res->user_id);
  
  u_snprintf(sql_str, sizeof(sql_str),
      "update public.buffs set "
        "luck_acorn = %d, "
        "proficiency_acorn = %d, "
        "boosted = %d \n"
      "where user_id = %ld; \n",
      player_res->buffs.luck_acorn, player_res->buffs.proficiency_acorn, player_res->buffs.boosted_acorn, 
      player_res->user_id);
  
  u_snprintf(sql_str, sizeof(sql_str),
      "update public.biome_story set "
        "grasslands = %d, "
        "seeping_sands = %d, "
        "nature_end = %d, "
        "death_grip = %d, "
        "last_acorn = %d "
      "where user_id = %ld; \n",
      player_res->story.grasslands, player_res->story.seeping_sands, player_res->story.nature_end,
      player_res->story.death_grip, player_res->story.last_acorn, 
      player_res->user_id);

  u_snprintf(sql_str, sizeof(sql_str),
      "update public.session_data set "
        "start_time = %ld, "
        "acorn_count = %d, "
        "acorns = %d, "
        "golden_acorns = %d, "
        "health_loss = %d, "
        "no_acorns = %d, "
        "acorn_handful = %d, "
        "acorn_mouthful = %d, "
        "lost_stash = %d, "
        "acorn_sack = %d "
      "where user_id = %ld; \n"
      "COMMIT; \n",
      (unsigned long)time(NULL), player_res->session_data.acorn_count, player_res->session_data.acorns,
      player_res->session_data.golden_acorns, player_res->session_data.health_loss, player_res->session_data.no_acorns, 
      player_res->session_data.acorn_handful, player_res->session_data.acorn_mouthful, player_res->session_data.lost_stash, 
      player_res->session_data.acorn_sack,
      player_res->user_id);

  PGresult* player_update = (PGresult*) { 0 };
  player_update = SQL_query(player_update, sql_str);
  PQclear(player_update);
}

void update_scurry_row(struct sd_scurry *scurry_res)
{
  char sql_str[1028] = { };

  u_snprintf(sql_str, sizeof(sql_str),
      "update public.scurry set "
        "total_stolen_acorns = %d, "
        "prev_stolen_acorns = %d, "
        "war_acorns = %d, "
        "war_flag = %d \n"
      "where owner_id = %ld \n",
      scurry_res->total_stolen_acorns, scurry_res->prev_stolen_acorns, scurry_res->war_acorns, 
      scurry_res->war_flag,
      scurry_res->scurry_owner_id);
    
  PGresult* scurry_update = (PGresult*) { 0 };
  scurry_update = SQL_query(scurry_update, sql_str);
  PQclear(scurry_update);
}