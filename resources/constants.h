
#define genrand(base, range) ( (rand() % (range +1)) + base ) // range +1 since rand is [base, range)

#define ADD_TO_BUFFER(buffer, size, format, ...) \
    snprintf(buffer + strlen(buffer), size - strlen(buffer), format, ##__VA_ARGS__)

/* @@@ SERVER CONSTANTS @@@ */
#define GUILD_ID 905167903224123473
#define OWNER_ID 582648847881338961

// PATH CONSTANTS
#define GIT_PATH "https://github.com/Furmissile/squirrel/blob/main/src_assets/%s?raw=true"
#define SQ_CHEM_PATH "sd_utils/sq_alchemist.png"
#define SQ_BOOKIE_PATH "sd_utils/squirrel_bookie.png"
#define WELCOME_MSG_PATH "sd_utils/welcome_gif.gif"
#define RULES_BK_PATH "sd_utils/rules_book.png"
#define BUNNY_ED_PATH "sd_utils/bunny.png"
#define VERIFY_PLUS_PATH "symbols/plus.png"

// SUPPORT SERVER SPECIFIC IDS
#define WELCOME_CHANNEL_ID 1046634205729275955
#define VERIFY_CHANNEL_ID 1046813534790635550
#define MEMBER_ROLE_ID 1046627142345170984

#define ACTION_SUCCESS 0x00ff00
#define ACTION_FAILED 0xff0000
#define ACTION_UNDEFINED 0x0

#define SCURRY_MEMBER_MAX 15
#define SCURRY_MEMBER_REQ 3 // requirement to participate in wars

#define SCURRY_CREATION_COST 50000 //50,000

// RANK REQUIREMENTS
#define SEED_NOT_MAX 5000
#define ACORN_SNATCHER_MAX 10000
#define SEED_SNIFFER_MAX 25000
#define OAKFFICIAL_MAX 50000
// Royal Nut max -> infinity

// WAR ACORN CAPS PER RANK
#define SEED_NOT_CAP 500
#define ACORN_SNATCHER_CAP 1000
#define SEED_SNIFFER_CAP 2500
#define OAKFFICIAL_CAP 5000
#define ROYAL_NUT_CAP 10000

// Multiply into rank
#define BASE_COURAGE_MULT 0.05f

#define MAX_ENERGY 100
#define MAX_HEALTH 100

/* @@@ COOLDOWNS @@@ */
#define COOLDOWN 2
#define BASE_ENERGY_CD 180 //3 minutes/ energy
#define INVITE_CD 120
#define MAIN_ENERGY_COST 2
#define STEAL_ENERGY_COST 20
#define STEAL_CHANCE 30
#define STEAL_MINIMUM 500


/* @@@ CHANCE DATA @@@ */
#define JUNK_CHANCE      10  // 10 % -- No acorns
#define COMMON_CHANCE    60  // 50 % -- Handful of acorns
#define UNCOMMON_CHANCE  80  // 20 % -- Mouthful of acorns 
#define CONTAINER_CHANCE 95  // 15 % -- Lost Stash
#define MAX_CHANCE       100 // 5  % -- Sack of acorns

#define ENCOUNTER_CHANCE 20 // 20 % -- checked separately

#define NORMAL_CHANCE 60      // 60 % -- Normal Reward
#define HEALTH_LOSS_CHANCE 90 // 30 % -- Health Loss
#define DOUBLE_REWARD 100     // 10 % -- Double Reward

#define VICTUALS_CHANCE 80

#define BLUEBERRY_CHANCE 75 // Gives acorn count
#define CHERRY_CHANCE 90 // Gives health
#define SEED_CHANCE 100 // Gives energy

#define BIOME_INTERVAL 5000
#define BIOME_DAMAGE 10
#define BIOME_ACORN_INC 10
#define BIOME_GOLDEN_ACORN_INC 5
#define DIVIDEND_VALUE 0.05f
#define ACORN_PRESTIGE 2 // score is halved on death
#define PRESTIGE_REQ 25000 // minimin acorn for prestige to happen

#define SQUIRREL_BOOST_INDEX 5
#define SQUIRREL_BOOST_COST 5

/* @@@ INTERACTION TYPES  @@@ */
#define TYPE_MAIN_MSG 'r'
#define TYPE_ENCOUNTER_MSG 'e'

#define TYPE_FORAGE 'f'
#define TYPE_UPGRADE 's'
#define TYPE_SQUIRREL 'q'
#define TYPE_BUNNY 'y'
#define TYPE_E_ACORN 'a'
#define TYPE_INVITE 'i'
#define TYPE_SCURRY_INFO 'x'
#define TYPE_LEADERBOARD 'l'

#define TYPE_PLAYER_HELP 'p'
#define TYPE_SCURRY_HELP 'c'
#define TYPE_EVENT_HELP 'v'
#define TYPE_BIOME_STORY 'b'


/* @@@ EMBED BUFFER SIZES @@@ */
#define SIZEOF_TITLE 256         // Includes field name, author name, and title

#define SIZEOF_DESCRIPTION 4096

#define SIZEOF_FIELD_VALUE 1024

#define SIZEOF_FOOTER_TEXT 2048

#define SIZEOF_CUSTOM_ID 64

#define SIZEOF_SQL_COMMAND 1024

#define SIZEOF_URL 1024

#define SIZEOF_SCURRY_NAME 16


/* @@@ STAT CONSTANTS @@@ */
#define STAT_EVOLUTION 5

#define BASE_HEALTH_REGEN 5

#define GOLDEN_ACORN_BUFF_COST 2000

#define COLOR_COST 1500

/* Price multiplier factors */
#define PROFICIENCY_UNIT 1500
#define LUCK_UNIT 5000
#define STRENGTH_UNIT 10000

/* @@@ The following stats are to increase resource earning, NOT to collect them @@@*/

// See generate_factor() for details
#define PROFICIENCY_VALUE 0.1f
#define LUCK_VALUE 0.1f
#define STRENGTH_VALUE 100
 
// Stat level brackets (for some style)
#define BRONZE_BRACKET 20
#define SILVER_BRACKET 40
#define GOLD_BRACKET 50

// SEASON MULTIPLIERS
#define SPRING_MULT 1.2f
#define SUMMER_MULT 1.1f
#define FALL_MULT 1.05f
#define WINTER_MULT 1 // Explicit for definition

#define SKELETAL_SQUIRREL_COUNT 25000
#define BOOKIE_SQUIRREL_COUNT 50000
#define KING_SQUIRREL_COUNT 100000

#define ERROR_STATUS -1

// Game errors
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

/* @@@ EMOJI CONSTANTS @@@ */
// *The following constants are sometimes used definitively

// Resources
#define ENERGY "<:energy:911642184397258752>"
#define ACORNS "<:acorns:1045028765392187402>"
#define ACORN_COUNT "<:passive_acorns:1050407923823677502>"
#define GOLDEN_ACORNS "<:golden_acorn:1045032005420728430>"
#define CONJURED_ACORNS "<:conjured_acorn:1102676336289521774>"
#define CATNIP "<:catnip:1052250368039452732>"

// Stats
#define IMMORTALITY_STAT "<:immortality:1072161685844021388>"
#define ARCANE_STAT "<:arcane:1072161681737793536>"
#define FINESSE_STAT "<:finesse:1072162409642463263>"

// Items
#define LOST_STASH "<:lost_stash:1044620525944705146>"

// Interface
#define QUEST_MARKER "<:quest_marker:1046493067500335104>"
#define HELP_MARKER "<:no_acorns:1044620527223975957>"
#define OFF_ARROW "<:offset_arrow:1010924151063715841>"
#define INDENT "<:empty_space:1019379796037337118>"

// Rates
#define BRONZE_STAHR "<:bronze_stahr:1010662617217511434>"
#define SILVER_STAHR "<:silver_stahr:1010662591615475835>"
#define STAHR "<:stahr:1045705606134251601>"

// Scurry
#define GUILD_ICON "<:guild_icon:1020714354351542362>"
#define LEADER "<:leader:1035976066965196861>"
#define COURAGE "<:courage:1045555306832347147>"
#define WAR_ACORNS "<:war_acorns:1066105193655193640>"
#define LIST_LEADER "<:king_squirrel:1073298108508803113>"
#define LIST_MEMBER "<:gl_squirrel:1014655900373504130>"
#define LIST_YOU "<:squirrel_bookie:1050175322286469250>"

// Health
#define HEALTH "<:health:1064579080281923655>"
#define BROKEN_HEALTH "<:broken_health:1064579097956712528>"

// Active Buffs
#define PROFICIENCY_ACORN "<:proficiency_acorn:1045027184475123813>"
#define LUCK_ACORN "<:luck_acorn:1045027183334264872>"
#define DEFENSE_ACORN "<:defense_acorn:1045027185754390578>"

// Biomes
#define GRASSLANDS_ICON "<:grasslands_icon:1007390631016017991>"
#define SEEPING_SANDS_ICON "<:sp_icon:1042082262566572153>"
#define NATURE_END_ICON "<:ne_icon:1071474392783142932>"
#define DEATH_GRIP_ICON "<:dg_icon:1092903646196412426>"
#define LAST_ACORN_ICON "<:la_icon:1105304340400451645>"
