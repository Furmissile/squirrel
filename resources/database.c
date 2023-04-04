
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

struct sd_player load_player_struct(unsigned long user_id)
{
  PGresult* search_player = SQL_query(conn, "select * from public.player where user_id = %ld",
    user_id);

  if (PQntuples(search_player) == 0)
  {  
    SQL_query(conn, "BEGIN; \n"
      "insert into public.player values(%ld, 0, 0, 0, 100, 100, 0, 0, 0, 0, 0, %d, 0); \n"
      "insert into public.stats values(%ld, 1, 1, 1); \n"
      "insert into public.buffs values(%ld, 0, 0, 0, 0, 0); \n"
      "insert into public.events values (%ld, 0); \n"
      "COMMIT;", 
      user_id, ERROR_STATUS, // ERROR_STATUS so an encounter doesnt trigger
      user_id, user_id, user_id);
  }
  PQclear(search_player);

  search_player = SQL_query(conn, "select * from public.player \
    join public.stats on player.user_id = stats.user_id \
    join public.buffs on player.user_id = buffs.user_id \
    join public.events on player.user_id = events.user_id \
    where player.user_id = %ld",
    user_id);

  player = (struct sd_player) { 0 };
  buff_status = (struct sd_buff_status) { 0 };

  struct sd_player player_res = (struct sd_player)
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
    .stolen_acorns = strtoint( PQgetvalue(search_player, 0, DB_STOLEN_ACORNS) ),
  
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
      .defense_acorn = strtoint( PQgetvalue(search_player, 0, DB_DEFENSE_ACORN) ),
      .strength_acorn = strtoint( PQgetvalue(search_player, 0, DB_STRENGTH_ACORN) ),
      .endurance_acorn = strtoint( PQgetvalue(search_player, 0, DB_ENDURANCE_ACORN) )
    },

    .events = {
      .catnip = strtoint( PQgetvalue(search_player, 0, DB_CATNIP) )
    }
  };

  player_res.biome = player_res.acorn_count/BIOME_INTERVAL % BIOME_SIZE;
  player_res.biome_num = player_res.acorn_count/BIOME_INTERVAL;
  player_res.max_health = generate_factor(player_res.stats.strength_lv, STRENGTH_VALUE) + MAX_HEALTH;

  PQclear(search_player);

  return player_res;
}

struct sd_scurry load_scurry_struct(unsigned long scurry_id)
{
  struct sd_scurry scurry_res = (struct sd_scurry) { 0 };

  if (scurry_id > 0)
  {
    PGresult* scurry_db = SQL_query(conn, "select * from public.scurry where owner_id = %ld", scurry_id);

    scurry = (struct sd_scurry) { 0 };

    scurry_res = (struct sd_scurry) 
    {
      .scurry_owner_id = strtobigint( PQgetvalue(scurry_db, 0, DB_SCURRY_OWNER_ID) ),
      .scurry_name = format_str(32, PQgetvalue(scurry_db, 0, DB_SCURRY_NAME) ),
      .courage = strtoint( PQgetvalue(scurry_db, 0, DB_COURAGE) ),
      .war_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_ACORNS) ),
      .war_flag = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_FLAG) ),
      .rank = (scurry_res.courage < SEED_NOT_MAX) ? SEED_NOT
          : (scurry_res.courage < ACORN_SNATCHER_MAX) ? ACORN_SNATCHER
          : (scurry_res.courage < SEED_SNIFFER_MAX) ? SEED_SNIFFER
          : (scurry_res.courage < OAKFFICIAL_MAX) ? OAKFFICIAL : ROYAL_NUT,
      
      .war_acorn_cap = (scurry_res.courage < SEED_NOT_MAX) ? SEED_NOT_CAP
          : (scurry_res.courage < ACORN_SNATCHER_MAX) ? ACORN_SNATCHER_CAP
          : (scurry_res.courage < SEED_SNIFFER_MAX) ? SEED_SNIFFER_CAP
          : (scurry_res.courage < OAKFFICIAL_MAX) ? OAKFFICIAL_CAP : ROYAL_NUT_CAP
    };

    if (scurry_res.courage < SEED_NOT_MAX)
    {
      scurry_res.rank = SEED_NOT;
      scurry_res.war_acorn_cap = SEED_NOT_CAP;
    }
    else if (scurry_res.courage < ACORN_SNATCHER_MAX)
    {
      scurry_res.rank = ACORN_SNATCHER;
      scurry_res.war_acorn_cap = ACORN_SNATCHER_CAP;
    }
    else if (scurry_res.courage < SEED_SNIFFER_MAX)
    {
      scurry_res.rank = SEED_SNIFFER;
      scurry_res.war_acorn_cap = SEED_SNIFFER_CAP;
    }
    else if (scurry_res.courage < OAKFFICIAL_MAX)
    {
      scurry_res.rank = OAKFFICIAL;
      scurry_res.war_acorn_cap = OAKFFICIAL_CAP;
    }
    else {
      scurry_res.rank = ROYAL_NUT;
      scurry_res.war_acorn_cap = ROYAL_NUT_CAP;
    }
    
    PQclear(scurry_db);
  }

  return scurry_res;
}

void update_player_row(struct sd_player player_res)
{
  char* sql_str = format_str(SIZEOF_SQL_COMMAND,
    "BEGIN; \
    update public.player set \
      scurry_id = %ld, \
      color = %d, \
      squirrel = %d, \
      energy = %d, \
      health = %d, \
      acorns = %d, \
      acorn_count = %d, \
      high_acorn_count = %d, \
      golden_acorns = %d, \
      stolen_acorns = %d, \
      encounter = %d, \
      main_cd = %ld \
    where user_id = %ld;",
      player_res.scurry_id, player_res.color, player_res.squirrel, player_res.energy, 
      player_res.health, player_res.acorns, player_res.acorn_count, player_res.high_acorn_count, 
      player_res.golden_acorns, player_res.stolen_acorns, player_res.encounter, player_res.main_cd,
      player_res.user_id);
    
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.stats set \
      proficiency_lv = %d, \
      strength_lv = %d, \
      luck_lv = %d \
    where user_id = %ld;",
      player_res.stats.proficiency_lv, player_res.stats.strength_lv, player_res.stats.luck_lv,
      player_res.user_id);
  
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.buffs set \
      defense_acorn = %d, \
      luck_acorn = %d, \
      proficiency_acorn = %d, \
      strength_acorn = %d, \
      endurance_acorn = %d \
    where user_id = %ld;",
      player_res.buffs.defense_acorn, player_res.buffs.luck_acorn,
      player_res.buffs.proficiency_acorn, player_res.buffs.strength_acorn, 
      player_res.buffs.endurance_acorn,
      player_res.user_id);
  
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.events set \
      catnip = %d \
    where user_id = %ld; \
    COMMIT;",
    player_res.events.catnip, 
    player_res.user_id);

  SQL_query(conn, sql_str);

  free(sql_str);
}

void update_scurry_row(struct sd_scurry scurry_res)
{
  char* sql_str = format_str(SIZEOF_SQL_COMMAND,
    "update public.scurry set \
        courage = %d, \
        war_acorns = %d, \
        war_flag = %d \
      where owner_id = %ld",
        scurry_res.courage, scurry_res.war_acorns, scurry_res.war_flag,
        scurry_res.scurry_owner_id);
  
  PQexec(conn, sql_str);

  free(sql_str);
}
