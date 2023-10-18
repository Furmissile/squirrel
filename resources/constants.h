// SERVER CONSTANTS
  #define GUILD_ID 905167903224123473
  #define OWNER_ID 582648847881338961


// OPERATION
  #define genrand(base, range) ( (rand() % (range +1)) + base ) // range +1 since rand is on [base, range)

// PATH CONSTANTS
  #define GIT_PATH "https://github.com/Furmissile/squirrel/blob/main/src_assets/%s?raw=true"
  #define SQ_CHEM_PATH "sd_utils/sq_alchemist.png"
  #define SQ_BOOKIE_PATH "sd_utils/squirrel_bookie.png"
  #define WELCOME_MSG_PATH "sd_utils/welcome_gif.gif"
  #define RULES_BK_PATH "sd_utils/rules_book.png"
  #define BUNNY_ED_PATH "sd_utils/bunny.png"
  #define VERIFY_PLUS_PATH "symbols/plus.png"


// SUPPORT SERVER SPECIFIC IDS
  #define RULES_CHANNEL_ID 1046640388456321126
  #define WELCOME_CHANNEL_ID 1046634205729275955
  #define VERIFY_CHANNEL_ID 1046813534790635550
  #define BETA_MEMBER_ROLE 1017203719600885810
  #define MEMBER_ROLE_ID 1046627142345170984


// STATUS COLORS
  #define ACTION_SUCCESS 0x00ff00
  #define ACTION_FAILED 0xff0000
  #define ACTION_UNDEFINED 0x0


// COOLDOWNS
  #define COOLDOWN 2
  #define BASE_ENERGY_CD 180 // 3 minutes/ energy
  #define INVITE_CD 120
  #define SESSION_CD 120


// CHANCE DATA
  #define JUNK_CHANCE      10  // 10 % -- No acorns
  #define COMMON_CHANCE    60  // 50 % -- Handful of acorns
  #define UNCOMMON_CHANCE  80  // 20 % -- Mouthful of acorns 
  #define CONTAINER_CHANCE 95  // 15 % -- Lost Stash
  #define MAX_CHANCE       100 // 5  % -- Sack of acorns

  #define ENCOUNTER_CHANCE 20 // 20 % -- checked separately
  #define STORY_CHANCE 80     // 20 %

  #define STEAL_CHANCE 30

  #define VICTUALS_CHANCE 80

  #define BLUEBERRY_CHANCE 75 // Gives acorn count
  #define CHERRY_CHANCE 90    // Gives health
  #define SEED_CHANCE 100     // Gives energy

  #define WAR_STEAL_CHANCE 50

  #define LUCK_BUFF_CHANCE 90
  #define PROFICIENCY_BUFF_CHANCE 75


// GENERAL PROGRESSION
  #define BIOME_INTERVAL 5000
  #define BIOME_ENCOUNTER_COST 100
  #define BIOME_ACORN_INC 0.1f
  #define STEAL_MINIMUM 1000


// INTERACTION TYPES
  #define TYPE_FORAGE_INIT 'n'
  #define TYPE_FORAGE_RESP 'r'
  #define TYPE_ENCOUNTER_RESP 'e'

  #define TYPE_FORAGE 'f'
  #define TYPE_INFO 'd'
  #define TYPE_INFO_FROM_BUTTONS 'h'
  #define TYPE_UPGRADE 'u'
  #define TYPE_SQUIRREL 's'
  #define TYPE_BUNNY 'g'
  #define TYPE_E_ACORN 'a'
  #define TYPE_INVITE 'i'
  #define TYPE_SCURRY_INFO 'x'
  #define TYPE_LEADERBOARD 'l'
  #define TYPE_SESSION_INFO 'j'

  #define TYPE_PLAYER_HELP 'p'
  #define TYPE_SCURRY_HELP 'c'
  #define TYPE_EVENT_HELP 'v'
  #define TYPE_BIOME_STORY 'b'


// FIXED COSTS
  #define MAIN_ENERGY_COST 2
  #define STEAL_ENERGY_COST 20


// MAX VALUES
  #define MAX_HEALTH 10
  #define MAX_ENERGY 100


// BUFFS
  #define HEALING_FACTOR 0.5f
  #define SQUIRREL_BOOST_COST 10
  #define BUFF_FACTOR 100

  #define GRAY_BOOST_DURATION 10 // based on energy loss
  #define SKELETAL_BOOST_DURATION 5 // based on health regen
  #define BOOKIE_BOOST_DURATION 15 // based on acorns
  #define ANGELIC_BOOST_DURATION 10 // based on health loss
  #define KING_BOOST_DURATION 15 // based on acorn count

  // explicitly defined to avoid confusion
  #define BUFF_CONSTANT 2
  #define SQUIRREL_CONSTANT 1.5f
  #define BOOSTED_ACORN_CONSTANT 1.5f

// SEASONS
  #define SPRING_MULT 1.2f
  #define SUMMER_MULT 1.1f
  #define FALL_MULT 1.05f
  #define WINTER_MULT 1 // Explicit for definition


// STATS
  #define STAT_EVOLUTION 5
  #define STAT_DIFFERENCE 2 // required stat difference between stats to upgrade
  #define BASE_HEALTH_REGEN 2

  // Stat multiplier factors - See generate_factor() for details
  #define PROFICIENCY_FACTOR 0.1f
  #define LUCK_FACTOR 0.1f
  #define STRENGTH_FACTOR 5

  // Price multiplier factors - See generate_price() for details
  #define PROFICIENCY_UNIT 1500
  #define LUCK_UNIT 5000
  #define STRENGTH_UNIT 15000


// SCURRY
  #define SCURRY_MEMBER_MAX 15
  #define SCURRY_MEMBER_REQ 4 // requirement to participate in wars
  #define SCURRY_CREATION_COST 50000
  #define WAR_STASH_FACTOR 250
  #define DEFAULT_WAR_STASH 1000

  // Rank Requirements
  #define SEED_NOT_MAX 10000
  #define ACORN_SNATCHER_MAX 20000
  #define SEED_SNIFFER_MAX 35000
  #define OAKFFICIAL_MAX 50000
  // Royal Nut max -> infinity

  // WAR ACORN CAPS PER RANK
  #define SEED_NOT_CAP 500
  #define ACORN_SNATCHER_CAP 1000
  #define SEED_SNIFFER_CAP 2500
  #define OAKFFICIAL_CAP 5000
  #define ROYAL_NUT_CAP 10000

  // Multiply into rank
  #define BASE_ACORN_MULT 0.05f

  #define GRAY_SQUIRREL_COUNT 0
  #define SKELETAL_SQUIRREL_COUNT 25000
  #define BOOKIE_SQUIRREL_COUNT 50000
  #define ANGELIC_SQUIRREL_COUNT 100000
  #define KING_SQUIRREL_COUNT 150000

// GAME ERRORS
  #define ERROR_STATUS -1

  #define ERROR_INTERACTION(error, message) \
      if (error) { \
        error_message(event, message); \
        return ERROR_STATUS; \
      }

  // Errors involving clearing a query
  #define ERROR_DATABASE_RET(error, message, PQ_result) \
      if (error) { \
        error_message(event, message); \
        PQclear(PQ_result); \
        return ERROR_STATUS; \
      }

// Util macros
#define APPLY_NUM_STR(buffer, value) \
    char buffer[64] = { }; \
    num_str(buffer, sizeof(buffer), value);

#define LOG_COMMAND(name) \
    FILE* fp = fopen("crash.log", "a+"); \
    fprintf(fp, "%s\n", name); \
    fclose(fp);

// EMOJI CONSTANTS
  // Resources
  #define ENERGY "<:energy:911642184397258752>"
  #define ACORNS "<:acorns:1045028765392187402>"
  #define ACORN_COUNT "<:acorn_count:1050407923823677502>"
  #define GOLDEN_ACORNS "<:golden_acorn:1045032005420728430>"
  #define CONJURED_ACORNS "<:conjured_acorn:1102676336289521774>"
  #define CATNIP "<:catnip:1052250368039452732>"

  // Rewards
  #define NO_ACORNS "<:no_acorns:1044620527223975957>"
  #define ACORN_HANDFUL "<:acorn_handful:1044620522236944384>"
  #define ACORN_MOUTHFUL "<:acorn_mouthful:1044620523067416627>"
  #define LOST_STASH "<:lost_stash:1044620525944705146>"
  #define ACORN_SACK "<:acorn_sack:1044620524275380286>"

  // Health
  #define HEALTH "<:health:1064579080281923655>"
  #define BROKEN_HEALTH "<:broken_health:1064579097956712528>"

  // Stats
  #define PROFICIENCY_ICON "<:proficiency:1066084547931947141>"
  #define LUCK_ICON "<:luck:1066084537798500372>"
  #define STRENGTH_ICON "<:strength:1066084556110831697>"

  // Active Buffs
  #define PROFICIENCY_ACORN "<:proficiency_acorn:1045027184475123813>"
  #define LUCK_ACORN "<:luck_acorn:1045027183334264872>"
  #define BOOSTED_ACORN "<:boosted_acorn:1119273554886799450>"
  
  // Interface
  #define QUEST_MARKER "<:quest_marker:1046493067500335104>"
  #define HELP_MARKER "<:no_acorns:1044620527223975957>"
  #define OFF_ARROW "<:offset_arrow:1010924151063715841>"
  #define INDENT "<:empty_space:1019379796037337118>"
  #define BULLET "<:bullet:1150089464161517621>"
  #define STAHR "<:stahr:1045705606134251601>"

  // Scurry
  #define GUILD_ICON "<:guild_icon:1020714354351542362>"
  #define LEADER "<:leader:1035976066965196861>"
  #define WAR_ACORNS "<:war_acorns:1066105193655193640>"
  #define LIST_LEADER "<:king_squirrel:1073298108508803113>"
  #define LIST_MEMBER "<:gl_squirrel:1014655900373504130>"
  #define LIST_YOU "<:squirrel_bookie:1050175322286469250>"

  // Biomes
  #define GRASSLANDS_ICON "<:grasslands_icon:1007390631016017991>"
  #define SEEPING_SANDS_ICON "<:sp_icon:1042082262566572153>"
  #define NATURE_END_ICON "<:ne_icon:1071474392783142932>"
  #define DEATH_GRIP_ICON "<:dg_icon:1092903646196412426>"
  #define LAST_ACORN_ICON "<:la_icon:1105304340400451645>"

  // Victuals
  #define BLUEBERRY_VICTUALS "<:blueberry_victuals:1094282921944305724>"
  #define CHERRY_VICTUALS "<:cherry_victuals:1094282923806556251>"
  #define SEED_VICTUALS "<:seed_victuals:1094282924750274572>"


// COMMAND CONSTANTS
  // make differences between beta and main automatic!!
  #if (APPLICATION_ID == MAIN_BOT_ID)
    #define STATUS_CHANNEL 1046635264883294259
    #define FORAGE_ID "</forage:1089663881959460926>"
    #define BIOME_STORY_ID "</biome_story:1105520586706403379>"
    #define INFO_ID "</info:1089663884673167440>"
    #define SQUIRRELS_ID "</squirrels:1089663967460327525>"
    #define COLOR_ID "</color:1089663885692370954>"
    #define STEAL_ID "</steal:1089663966520819865>"
    #define SCURRY_INFO_ID "</scurry_info:1089664056320852000>"
    #define SCURRY_HELP_ID "</scurry_help:1089663969721065592>"
    #define SCURRY_KICK_ID "</scurry_kick:1089664058225066155>"
    #define SCURRY_LEAVE_ID "</scurry_leave:1089664058879377548>"
    #define SEASON_INFO_ID "</season_info:1089664143419768972>"
    #define BUNNY_ENDEAVOR_ID "</bunny_endeavor:1089664142203424769>"
  #else
    #define STATUS_CHANNEL 1049184118954528888
    #define FORAGE_ID "</forage:1089659245588381716>"
    #define BIOME_STORY_ID "</biome_story:1103730882092748940>"
    #define INFO_ID "</info:1089659248214024332>"
    #define SQUIRRELS_ID "</squirrels:1089659332003635220>"
    #define COLOR_ID "</color:1089659248595714111>"
    #define STEAL_ID "</steal:1089659330674036766>"
    #define SCURRY_INFO_ID "</scurry_info:1089659416749539348>"
    #define SCURRY_HELP_ID "</scurry_help:1089659334490853488>"
    #define SCURRY_KICK_ID "</scurry_kick:1089659418876059669>"
    #define SCURRY_LEAVE_ID "</scurry_leave:1089659419782037514>"
    #define SEASON_INFO_ID "</season_info:1089659504276283492>"
    #define BUNNY_ENDEAVOR_ID "</bunny_endeavor:1089659502573387836>"
  #endif