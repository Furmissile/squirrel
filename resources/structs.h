struct sd_file_data
{
  char* formal_name;
  char* file_path;

  char* description;

  char* emoji_name;
  unsigned long emoji_id;
};

struct sd_squirrel
{
  int acorn_count_req;
  int boosted_duration;
  struct sd_file_data squirrel;
};

struct sd_designer_squirrel
{
  int golden_acorn_req;
  struct sd_file_data squirrel;
};

struct sd_encounter
{
  char* name;
  char* file_path;
  char* conflict;
  char* solutions[3];
  char* context;
};

struct sd_biome_section
{
  char* section_name;
  struct sd_encounter *encounters;
  int section_size;
};

struct sd_biome
{
  char* biome_scene_path;
  struct sd_file_data biome_icon; // both emoji
  char* intro;

  struct sd_biome_section *sections;
  int biome_size;
};

struct sd_obj_stats
{
  struct sd_file_data stat;

  float price_mult;
  float value_mult;
};

struct sd_scurry 
{
  unsigned long scurry_owner_id;
  char scurry_name[64];

  int total_stolen_acorns;
  int prev_stolen_acorns;
  int war_acorns;

  int war_flag;
  int rank; // not in db

  int war_acorn_cap; // not in db
};

struct sd_stats 
{
  int proficiency_lv;
  int luck_lv;
  int strength_lv;
};

// strength and healing are not included
struct sd_buffs 
{
  int proficiency_acorn;
  int luck_acorn;
  int boosted_acorn; // boosted squirrel duration
};

struct sd_buff_status
{
  bool proficiency_acorn;
  bool luck_acorn;
  bool boosted_acorn;

  bool received_proficiency_buff;
  bool received_luck_buff;
};

struct sd_biome_story
{
  int grasslands;
  int seeping_sands;
  int nature_end;
  int death_grip;
  int last_acorn;
};

struct sd_session_data
{
  unsigned long start_time;
  int acorn_count;
  int acorns;
  int golden_acorns;
  int health_loss;

  int total_forages;
  int no_acorns;
  int acorn_handful;
  int acorn_mouthful;
  int lost_stash;
  int acorn_sack;
};

struct sd_player 
{
  unsigned long user_id;
  unsigned long scurry_id;
  int color;
  int squirrel;
  int designer_squirrel;
  int squirrel_lv;
  float squirrel_const;
  unsigned long purchased;

  int energy;
  int health;
  int max_health;

  int acorns;
  int acorn_count;
  int high_acorn_count;
  int golden_acorns;
  int spent_golden_acorns;
  int conjured_acorns;
  int stolen_acorns;
  int catnip;
  
  int encounter; // biome-relative encounter idx
  int biome; // current biome the player is on
  int section; // current section player is on
  int biome_num; // total number of biomes completed

  time_t main_cd;
  int button_idx;

  struct sd_stats stats;
  struct sd_buffs buffs;
  struct sd_biome_story story;
  struct sd_session_data session_data;
};

struct sd_store
{
  int item_idx;
  int cost;
  int quantity;
  int* stat_ptr;
};

struct sd_rewards
{
  int item_type;

  int acorns;
  int acorn_count;
  int golden_acorns;
  int conjured_acorns;

  int war_acorns; // for building scurry stash
  int stolen_acorns;

  int encounter_cost;
  int is_health;
  int charge_ready;

  int seasoned_golden_acorns;
  int catnip;
  int victual_amt;
  int victual_type;
  int ref_resource;
};

// For listing members...
struct sd_db_info 
{
  int db_idx;
  char username[128];
  unsigned long user_id;
  int value;
};

struct sd_header_params 
{
  struct discord_embed embed;
  char username[64];
  char avatar_url[128];
  char title[64];
  char thumbnail_url[128];
};

struct sd_util_info 
{
  struct discord_component buttons[5];
  char custom_ids[5][64];
  char labels[5][64];

  struct discord_emoji emojis[5];
  char emoji_names[5][64];
};

struct sd_help_info 
{
  struct discord_component buttons[4];
  char custom_ids[4][64];

  struct discord_emoji emojis[4];
  char emoji_names[4][64];
  
  struct discord_embed_field *fields;
  char (*field_names)[64];
  char (*field_values)[1024];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

struct sd_welcome_info 
{
  char description[512];
  char image_url[128];

  char footer_text[256];
  char footer_url[128];
};