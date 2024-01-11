// SERVER CONSTANTS
  #define GUILD_ID 905167903224123473
  #define OWNER_ID 582648847881338961


  // differences between beta and main automatic!!
  #if (APPLICATION_ID == MAIN_BOT_ID)
    #define STATUS_CHANNEL 1046635264883294259

    #define START_ID "</start:1182163458133864571>"
    #define COLOR_ID "</color:1182163461715808276>"
    #define SCURRY_CREATE_ID "</scurry_create:1182163462688886926>"
    #define SCURRY_DISBAND_ID "</scurry_disband:1182163544687530096>"
    #define SCURRY_KICK_ID "</scurry_kick:1182163543408246935>"
    #define SCURRY_RENAME_ID "</scurry_rename:1182163545723523072>"
  #else
    #define STATUS_CHANNEL 1049184118954528888

    #define START_ID "</start:1178372476045103194>"
    #define COLOR_ID "</color:1089659248595714111>"
    #define SCURRY_CREATE_ID "</scurry_create:1089659500916650004>"
    #define SCURRY_DISBAND_ID "</scurry_disband:1179508464465215529>"
    #define SCURRY_KICK_ID "</scurry_kick:1179508461730537504>"
    #define SCURRY_RENAME_ID "</scurry_rename:1179414106051186688>"
  #endif


// OPERATION
  #define genrand(base, range) ( (rand() % (range +1)) + base ) // range +1 since rand is on [base, range)

// PATH CONSTANTS
  #define GIT_PATH "https://github.com/Furmissile/squirrel/blob/main/src_assets/%s?raw=true"
  #define RULES_BK_PATH "misc/rules_book.png"
  #define VERIFY_PLUS_PATH "misc/plus.png"


// STATUS COLORS
  #define ACTION_SUCCESS 0x00ff00
  #define ACTION_FAILED 0xff0000
  #define ACTION_UNDEFINED 0x0


// CHANCE
  #define COMMON_CHANCE 65 // 65% -- acorn
  #define EVENT_CHANCE 85 // 20% -- event resources
  #define RARE_CHANCE 95 // 10% -- golden acorn
  #define MAX_CHANCE 100 // 5% -- conjured acorn

  #define MAX_PIES 3

  #define ENCOUNTER_CHANCE 15 // 15%
  #define STORY_CHANCE 60     // 40%

  #define STEAL_CHANCE 50
  #define DOUBLE_STEAL 80
  #define STEAL_COST 15

  #define WAR_ACORN_CHANCE 80

  #define FIRST_DAILY 5
  #define SECOND_DAILY 15
  #define THIRD_DAILY 25

  #define DAILY_REWARD 10

// GENERAL PROGRESSION
  #define BIOME_INTERVAL 2000

// PIES
  #define EMPTY_PIE "00000000"
  #define EMPTY_HISTORY "%d..." // remembers the last 4 pieces
  #define PIE_FORMAT "%d%d%d%d%d%d%d%d"

// INTERACTION TYPES
  #define TYPE_ADD_PIECE 'a'
  #define TYPE_INFO 'b'
  #define TYPE_INFO_FROM_BUTTONS 'c'
  #define TYPE_SQUIRREL 'd'
  #define TYPE_SQUIRREL_HELP 'e'
  #define TYPE_INIT_STEAL 'f'
  #define TYPE_STEAL 'g'

  #define TYPE_PLAYER_HELP 'h'
  #define TYPE_SCURRY_HELP 'i'
  #define TYPE_EVENT_HELP 'j'
  #define TYPE_BIOME_STORY 'k'

  #define TYPE_INVITE 'l'
  #define TYPE_INIT_INVITE 'm'
  #define TYPE_SCURRY_INFO 'n'
  #define TYPE_LEADERBOARD 'o'
  #define TYPE_SCURRY_LEAVE 'p'


// SCURRY
  #define INVITE_CD 120
  #define SCURRY_MEMBER_MAX 5
  #define SCURRY_MEMBER_REQ 3 // requirement to participate in wars
  #define SCURRY_CREATION_COST 250 // in CONJURED ACORNS
  #define SCURRY_RANK_BONUS 1.25f

  // Rank Requirements (in war acorn)
  #define SEED_NOT_MAX 50
  #define OAKFFICIAL_MAX 100
  #define ROYAL_NUT_MAX 200
  // Royal Nut max -> infinity

  #define FIRST_SQUIRREL 25
  #define SECOND_SQUIRREL 50
  #define THIRD_SQUIRREL 100
  #define FOURTH_SQUIRREL 250

  #define CHRISTMAS_MONTH 11

// GAME ERRORS
  #define ERROR_STATUS -1

  #define ERROR_INTERACTION(error, message, ...) \
      if (error) { \
        error_message(event, message, ##__VA_ARGS__); \
        return ERROR_STATUS; \
      }

  // Errors involving clearing a query
  #define DATABASE_ERROR(error, message, PQ_result) \
      if (error) { \
        error_message(event, message); \
        PQclear(PQ_result); \
        return ERROR_STATUS; \
      }

// Util macros
#define APPLY_NUM_STR(buffer, value) \
    char buffer[64] = { }; \
    num_str(buffer, sizeof(buffer), value);

  enum DETECT_ACTION
  {
    DETECT_ROTATE = MAX_PIES,
    DETECT_RESTART,
    DETECT_START,
    DETECT_SIZE
  };

// EMOJI CONSTANTS
  // Resources
  #define ACORN_COUNT "<:acorn_count:1164894602491351113>"
  #define EMPTY_ACORN "<:empty_acorn:1191889906696474665>"
  #define ACORNS "<:acorn:1164894600926863360>"
  #define GOLDEN_ACORNS "<:golden_acorn:1164894605788074066>"
  #define CONJURED_ACORNS "<:conjured_acorn:1164897240771461151>"
  #define CATNIP "<:catnip:1164897238749818961>"

  #define EMPTY_BOX "<:empty:1191574115887349823>"
  #define COMMON_BOX "<:common_box:1191075902822547527>"
  #define RARE_BOX "<:rare_box:1191444823710908477>"
  #define UNIQUE_BOX "<:unique_box:1191444163837825118>"
  #define BONUS_BOX "<:bonus_box:1191444727195783278>"

  // Rewards
  #define RIBBONED_ACORN "<:ribboned_acorn:1180504068750311515>"
  #define COAL "<:coal:1180327193524580362>"

  // Interface
  #define QUEST_MARKER "<:quest_marker:1164898155150708736>"
  #define HELP_MARKER "<:no_acorns:1164898153884029012>"
  #define OFF_ARROW "<:offset_arrow:1010924151063715841>"
  #define INDENT "<:empty_space:1019379796037337118>"
  #define BULLET "<:bullet:1150089464161517621>"
  #define STAHR "<:stahr:1045705606134251601>"
  #define ADD "<:plus:1177068547973849130>"
  #define MINUS "<:minus:1177071484011819109>"
  #define WEATHER "<:weather:1178018949267476550>"
  #define TREASURE "<:lost_stash:1164898152906764349>"

  // Scurry
  #define GUILD_ICON "<:guild_icon:1164918417300668466>"
  #define LEADER "<:leader:1164918418844164186>"
  #define WAR_ACORNS "<:war_acorns:1164897241698418688>"
  #define LIST_LEADER "<:king_squirrel:1073298108508803113>"
  #define LIST_MEMBER "<:gray_squirrel:1014655900373504130>"
  #define LIST_YOU "<:squirrel_bookie:1050175322286469250>"

  // Biomes
  #define GRASSLANDS_ICON "<:gl_icon:1164900062091018240>"
  #define SEEPING_SANDS_ICON "<:sp_icon:1164900129850019860>"
  #define NATURE_END_ICON "<:ne_icon:1164900105615327243>"
  #define DEATH_GRIP_ICON "<:dg_icon:1164900043036299335>"
  #define LAST_ACORN_ICON "<:la_icon:1164900082315968542>"

  // Victuals
  #define BLUEBERRY_VICTUALS "<:blueberry_victuals:1164899712751636531>"
  #define CHERRY_VICTUALS "<:cherry_victuals:1164899714467115089>"
  #define SEED_VICTUALS "<:seed_victuals:1164899715456975131>"
