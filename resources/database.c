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

void load_player_struct(struct sd_player *player_res, unsigned long user_id)
{
  PGresult* search_player = (PGresult*) { 0 };
  search_player = SQL_query(search_player, 
      "select * from public.player where user_id = %ld",
      user_id);

  if (PQntuples(search_player) == 0)
  {
    PQclear(search_player);
    search_player = SQL_query(search_player,
        "BEGIN; \n"
        "insert into public.player values(%ld, 0, 0, 0, 100, 10, 0, 0, 0, 0, 0, %d, 0, 0, 0, 0); \n"
        "insert into public.stats values(%ld, 1, 1, 1); \n"
        "insert into public.buffs values(%ld, 0, 0, 0, 0, 0); \n"
        "COMMIT;", 
        user_id, ERROR_STATUS, // ERROR_STATUS so an encounter doesnt trigger
        user_id, user_id);
  }

  PQclear(search_player);

  search_player = SQL_query(search_player,
      "select * from public.player \n"
      "join public.stats on player.user_id = stats.user_id \n"
      "join public.buffs on player.user_id = buffs.user_id \n"
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
    .conjured_acorns = strtoint( PQgetvalue(search_player, 0, DB_CONJURED_ACORNS)),
    .stolen_acorns = strtoint( PQgetvalue(search_player, 0, DB_STOLEN_ACORNS) ),
    .catnip = strtoint( PQgetvalue(search_player, 0, DB_CATNIP) ),
  
    .vengeance_flag = strtoint( PQgetvalue(search_player, 0, DB_VENGEANCE_MODE) ),
    .encounter = strtoint( PQgetvalue(search_player, 0, DB_ENCOUNTER) ),
    .main_cd = strtobigint( PQgetvalue(search_player, 0, DB_MAIN_CD) ),

    .stats = {
      .proficiency_lv = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_LV) ),
      .strength_lv = strtoint( PQgetvalue(search_player, 0, DB_STRENGTH_LV) ),
      .luck_lv = strtoint( PQgetvalue(search_player, 0, DB_LUCK_LV) )
    },

    .buffs = {
      .proficiency_acorn = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_ACORN) ),
      .luck_acorn = strtoint( PQgetvalue(search_player, 0, DB_LUCK_ACORN) ),
      .strength_acorn = strtoint( PQgetvalue(search_player, 0, DB_STRENGTH_ACORN) ),
      .endurance_acorn = strtoint( PQgetvalue(search_player, 0, DB_ENDURANCE_ACORN) ),
      .boosted_acorn = strtoint( PQgetvalue(search_player, 0, DB_BOOSTED_ACORN) )
    }
  };

  player_res->biome = player_res->acorn_count/BIOME_INTERVAL % BIOME_SIZE;
  player_res->biome_num = player_res->acorn_count/BIOME_INTERVAL;
  player_res->max_health = generate_factor(player_res->stats.strength_lv, STRENGTH_FACTOR) + MAX_HEALTH;

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
  char sql_str[1028] = { };

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
        "conjured_acorns = %d, "
        "stolen_acorns = %d, "
        "catnip = %d, "
        "encounter = %d, "
        "vengeance_flag = %d, "
        "main_cd = %ld \n"
      "where user_id = %ld; \n",
      player_res->scurry_id, player_res->color, player_res->squirrel, player_res->energy, 
      player_res->health,player_res->acorns, player_res->acorn_count, player_res->high_acorn_count, 
      player_res->golden_acorns, player_res->conjured_acorns,player_res->stolen_acorns, player_res->catnip,
      player_res->encounter, player_res->vengeance_flag, player_res->main_cd,
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
        "strength_acorn = %d, "
        "endurance_acorn = %d, "
        "boosted = %d \n"
      "where user_id = %ld; \n"
      "COMMIT; \n",
      player_res->buffs.luck_acorn, player_res->buffs.proficiency_acorn, player_res->buffs.strength_acorn, 
      player_res->buffs.endurance_acorn, player_res->buffs.boosted_acorn, 
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