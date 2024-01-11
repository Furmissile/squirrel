struct sd_command
{
  char* name;
  char command_id; // custom id prefix (if one exists)

  int (*func_cb)(const struct discord_interaction *event);
};

enum APP_TYPE
{
  APP_START,
  APP_PLAYER,
  APP_COLOR,
  APPL_SCURR_CREATE,
  APP_SCURRY_RENAME,
  APP_SCURRY_DISBAND,
  APP_SIZE
};

struct sd_command *app_commands = (struct sd_command[])
{
  {
    .name = "start",
    .func_cb = &init_welcome_interaction
  },
  {
    .name = "Player",
    .func_cb = &info_interaction
  },
  {
    .name = "color",
    .func_cb = &color_interaction
  },
  {
    .name = "scurry_create",
    .func_cb = &create_interaction
  },
  {
    .name = "scurry_rename",
    .func_cb = &rename_interaction
  },
  {
    .name = "scurry_disband",
    .func_cb = &disband_interaction
  }
};

enum BUTTON_TYPE
{
  BUTTON_PLAYER_HELP,
  BUTTON_BIOME_STORY,
  BUTTON_MAIN_FORAGE,
  BUTTON_INFO,
  BUTTON_INFO_FROM_BUTTONS,
  BUTTON_INIT_STEAL,
  BUTTON_STEAL,
  BUTTON_SQUIRREL,
  BUTTON_HELP_SQUIRRELS,

  BUTTON_LEADERBOARD,

  BUTTON_EVENT_HELP,

  BUTTON_SCURRY_INFO,
  BUTTON_SCURRY_INIT_INVITE,
  BUTTON_SCURRY_INVITE_RESP,
  // BUTTON_SCURRY_HELP,
  BUTTON_SCURRY_LEAVE,

  BUTTON_SIZE
};

struct sd_command *button_types = (struct sd_command[])
{
  { // BUTTON_PLAYER_HELP
    .command_id = TYPE_PLAYER_HELP,
    .func_cb = &player_help_interaction,
  },
  { // BUTTON_BIOME_STORY
    .command_id = TYPE_BIOME_STORY,
    .func_cb = &biome_story_interaction
  },
  { // BUTTON_INIT_FORAGE -- when "Start" is pressed
    .command_id = TYPE_ADD_PIECE,
    .func_cb = &add_piece_interaction
  },
  { // BUTTON_INFO
    .command_id = TYPE_INFO,    
    .func_cb = &info_interaction
  },
  { // BUTTON_INFO_FROM_BUTTONS
    .command_id = TYPE_INFO_FROM_BUTTONS,
    .func_cb = &info_from_buttons
  },
  { // BUTTON_INIT_STEAL
    .command_id = TYPE_INIT_STEAL,    
    .func_cb = &steal_interaction
  },
  { // BUTTON_STEAL
    .command_id = TYPE_STEAL,    
    .func_cb = &steal_interaction
  },
  { // BUTTON_SQUIRREL
    .command_id = TYPE_SQUIRREL,
    .func_cb = &squirrels_interaction
  },
  { // BUTTON_HELP_SQUIRRELS
    .command_id = TYPE_SQUIRREL_HELP,
    .func_cb = &squirrels_interaction
  },
  { // BUTTON_LEADERBOARD
    .command_id = TYPE_LEADERBOARD,
    .func_cb = &fetch_leaderboard
  },
  // EVENT COMMANDS
  { // BUTTON_EVENT_HELP
    .command_id = TYPE_EVENT_HELP,
    .func_cb = &event_help_interaction
  },
  // SCURRY COMMANDS
  { // BUTTON_SCURRY_INFO
    .command_id = TYPE_SCURRY_INFO,
    .func_cb = &s_info_interaction
  },
  { // BUTTON_SCURRY_INIT_INVITE
    .command_id = TYPE_INIT_INVITE,
    .func_cb = &init_invite_interaction
  },
  { // BUTTON_SCURRY_INVITE_RESP
    .command_id = TYPE_INVITE,
    .func_cb = &invite_interaction
  },
  { // BUTTON_SCURRY_HELP
    .command_id = TYPE_SCURRY_HELP,    
    .func_cb = &scurry_help_interaction
  },
  { // BUTTON_SCURRY_LEAVE
    .command_id = TYPE_SCURRY_LEAVE,
    .func_cb = &leave_interaction
  }
};

void create_commands(struct discord *client, const struct discord_ready *event)
{
  (void)event;

  struct discord_create_global_application_command commands[] =
  {
    {
      .name = "start",
      .description = "Begin your adventure here!",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // user preferences
      .name = "Player",
      .type = DISCORD_APPLICATION_USER,
      .description = " "
    },
    { // user preferences
      .name = "Scurry",
      .type = DISCORD_APPLICATION_USER,
      .description = " "
    },
    {
      .name = "color",
      .description = "Set the color of your embed!",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "color",
            .description = "Enter a hex value (e.g. 3e4af2)",
            .required = true
          }
        },
        .size = 1
      }
    },
    {
      .name = "scurry_create",
      .description = "Create a scurry for 250 conjured acorns.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "name",
            .description = "Name your scurry (1 - 16 characters)",
            .required = true
          }
        },
        .size = 1
      }
    },
    {
      .name = "scurry_kick",
      .description = "Kick a scurry member.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "user",
            .description = "mention the user or use their ID",
            .required = true
          }
        },
        .size = 1
      }
    },
    {
      .name = "scurry_disband",
      .description = "DISBAND your scurry.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "verify",
            .description = "Enter the name of your scurry",
            .required = true
          }
        },
        .size = 1
      }
    },
    {
      .name = "scurry_rename",
      .description = "Rename your scurry for 250 conjured acorns. (Keep it SFW)",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "name",
            .description = "Name your scurry (1 - 16 characters)",
            .required = true
          }
        },
        .size = 1
      }
    }
  };

  for (int i = 0; i < (int)(sizeof(commands)/sizeof(*commands)); i++)
    discord_create_global_application_command(client, APPLICATION_ID, &commands[i], NULL);
}