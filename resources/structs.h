struct sd_file_data
{
  char* formal_name;
  char* file_path;

  char* description;

  char* emoji_name;
  unsigned long emoji_id;
};

struct sd_pie 
{
  char encoded_buf[64];
  int decoded_buf[SLICES_SIZE];
  char formatted_pie[1024];
};

struct sd_squirrel
{
  int cost;
  struct sd_file_data squirrel;
};

struct sd_slice_types
{
  struct sd_file_data item;
  char* emoji_literal;
  int base_value;
};

struct sd_encounter
{
  char* name;
  char* file_path;
  char* conflict;
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

struct sd_scurry 
{
  unsigned long scurry_owner_id;
  char scurry_name[64];

  int war_flag;
  int prev_stolen_acorns;
  int total_stolen_acorns;
  int war_acorns;
  int war_acorn_cap;
  int rank;
  int pie_count;
};

struct sd_biome_story
{
  int grasslands;
  int seeping_sands;
  int nature_end;
  int death_grip;
  int last_acorn;
};

struct sd_player 
{
  unsigned long user_id;
  unsigned long scurry_id;
  int color;
  int squirrel;
  unsigned long purchased;

  int energy;

  int treasury_filled;
  int high_acorn_count;
  int conjured_acorns;
  int stolen_acorns;

  int pies_complete;
  int tm_mday;
  
  int encounter; // biome-relative encounter idx
  int section; // current section player is on

  int biome; // current biome the player is on
  int biome_num; // total number of biomes completed

  unsigned long timestamp;
  int button_idx;

  struct sd_biome_story story;
};

struct sd_pie_game
{
  unsigned long user_id;
  int score;
  struct sd_pie pie_1;
  struct sd_pie pie_2;
  struct sd_pie pie_3;

  struct sd_pie current_piece;
  struct sd_pie next_piece;

  char history[64];
  int piece_count;
};

struct sd_header_params 
{
  struct discord_embed embed;
  char username[64];
  char avatar_url[128];
  char title[64];
  char thumbnail_url[128];
};

// For listing members...
struct sd_db_info 
{
  int db_idx;
  char username[128];
  unsigned long user_id;
  int value;
};

struct sd_util_info 
{
  struct discord_component buttons[5];
  char custom_ids[5][64];
  char labels[5][64];

  struct discord_emoji emojis[5];
  char emoji_names[5][64];

  int buttons_displayed;
};

struct sd_pages
{
  struct discord_component buttons[4];
  char custom_ids[4][64];
  char labels[4][64];

  struct discord_emoji emojis[4];
  char emoji_names[4][64];
};

struct sd_help_info 
{
  struct discord_embed_field field;
  char field_name[64];
  char field_value[1024];

  char description[512];
  char image_url[128];

  char footer_text[64];
  char footer_url[128];
};

struct sd_help_buttons
{
  struct discord_component buttons[3];
  char custom_ids[3][64];
  char labels[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];
};

struct sd_biome_info
{
  struct discord_embed_field *fields;
  char (*field_names)[64];
  char (*field_values)[1024];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

struct sd_statistics
{
  struct discord_component buttons[3];
  char custom_ids[3][64];
  char labels[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];
};

struct sd_welcome_info 
{
  char description[512];
  char image_url[128];

  char footer_text[256];
  char footer_url[128];
};