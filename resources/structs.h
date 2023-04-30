struct sd_file_data
{
  char* formal_name;
  char* file_path;

  char* description;

  char* emoji_name;
  unsigned long emoji_id;
};

// items that hold an independent position in player data
// includes items, enchanted acorns, and scurry items
struct sd_obj_items
{
  struct sd_file_data item;
  int *stat_ptr;
};

struct sd_squirrel
{
  int acorn_count_req;
  struct sd_file_data squirrel;
  struct sd_file_data evo_squirrel;
};

struct sd_obj_stats
{
  struct sd_file_data stat;
  int* stat_ptr;

  float price_mult;
  float value_mult;
};

struct sd_victuals
{
  struct sd_file_data item;
  int* stat_ptr;

  struct sd_obj_items *item_ref;
};

struct sd_encounter
{
  char* name;
  char* file_path;
  char* conflict;
  char* solutions[3];
};

struct sd_biome
{
  char* biome_scene_path;
  struct sd_file_data biome_icon; // both emoji

  struct sd_encounter *encounters;

  int encounter_size;
};

struct sd_scurry 
{
  unsigned long scurry_owner_id;
  char* scurry_name;

  int courage; // most recent score
  int war_acorns;

  int war_flag;
  int rank; // not in db

  int war_acorn_cap; // not in db
};

struct sd_stats 
{
  int proficiency_lv;
  int strength_lv;
  int luck_lv;
};

// strength and healing are not included
struct sd_buffs 
{
  int proficiency_acorn;
  int defense_acorn;
  int luck_acorn;
  int strength_acorn;
  int endurance_acorn;
};

struct sd_buff_status
{
  bool proficiency_acorn;
  bool defense_acorn;
  bool luck_acorn;
};

struct sd_events 
{
  int catnip;
};

struct sd_player 
{
  unsigned long user_id;
  unsigned long scurry_id;
  int color;
  int squirrel;

  int energy;
  int health;
  int max_health;

  int acorns;
  int acorn_count;
  int high_acorn_count;
  int golden_acorns;
  int war_acorns;
  
  int encounter;
  int biome;
  int biome_num;

  time_t main_cd;

  struct sd_stats stats;
  struct sd_buffs buffs;
  struct sd_events events;
};

struct sd_rewards
{
  int item_type;
  int has_responded;
  int acorns;
  int acorn_count;
  int health_loss;
  int failure; // responds to all 3 encounter buttons being TYPE_HEALTH_LOSS
  int health_regen;

  int courage;
  int war_acorns;

  int golden_acorns;
  
  int catnip;
  int victuals;
  int victual_type;

  int buff_duration;
};

struct sd_store
{
  struct sd_file_data *item;
  int* stat_ptr;

  int cost;
  int quantity;
};

struct sd_message
{
  struct discord_embed *embed;
  struct discord_components *buttons;

  char* content;
};