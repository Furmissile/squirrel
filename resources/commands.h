struct sd_command
{
  char* name;
  char command_id; // custom id prefix (if one exists)
  char* error_msg;

  int (*func_cb)(const struct discord_interaction *event);
};

enum CMD_TYPE
{
  CMD_PLAYER_HELP,
  CMD_BIOME_STORY,
  CMD_INIT_FORAGE,
  CMD_MAIN_FORAGE,
  CMD_ENCOUNTER_FORAGE,
  CMD_UPGRADE,
  CMD_BUFFS,
  CMD_INFO,
  CMD_INFO_FROM_BUTTONS,
  CMD_COLOR,
  CMD_STEAL,
  CMD_SQUIRREL,

  CMD_EVENT_HELP,
  CMD_SEASON_INFO,
  CMD_BUNNY_ENDEAVOR,

  CMD_LEADERBOARD,

  CMD_SCURRY_HELP,
  CMD_SCURRY_INFO,
  CMD_SCURRY_INVITE,
  CMD_SCURRY_INVITE_RESP,
  CMD_SCURRY_KICK,
  CMD_SCURRY_LEAVE,
  CMD_SCURRY_CREATE,

  CMD_SIZE
};

struct sd_command *cmds = (struct sd_command[])
{
  // MAIN COMMANDS
  { // CMD_PLAYER_HELP
    .name = "player_help",
    .command_id = TYPE_PLAYER_HELP,
    .error_msg = "This help embed belongs to someone else! Please send `/player_help` for support.",

    .func_cb = &player_help_interaction,
  },
  // CMD_BIOME_STORY
  {
    .name = "biome_story",
    .command_id = TYPE_BIOME_STORY,
    .error_msg = "This info embed belongs to someone else! Please send `/biome_story to read through it.",

    .func_cb = &biome_story_interaction
  },
  { // CMD_INIT_FORAGE -- in case "forage again" is pressed
    .name = "forage",
    .command_id = TYPE_FORAGE_INIT,
    .error_msg = "This is someone else's adventure! Please send `/forage` to start your own adventure!",

    .func_cb = &init_forage_interaction
  },
  { // CMD_MAIN_FORAGE
    .name = "forage",
    .command_id = TYPE_FORAGE_RESP,
    .error_msg = "This is someone else's adventure! Please send `/forage` to start your own adventure!",

    .func_cb = &forage_interaction
  },
  { // CMD_ENCOUNTER_FORAGE
    .name = "forage",
    .command_id = TYPE_ENCOUNTER_RESP,
    .error_msg = "This is someone else's adventure! Please send `/forage` to start your own adventure!",

    .func_cb = &encounter_interaction
  },
  { // CMD_UPGRADE
    .name = "upgrade",
    .command_id = TYPE_UPGRADE,
    .error_msg = "You cannot upgrade your stats with this embed! Please send `/upgrade` to access upgrades.",

    .func_cb = &init_upgrade_shop
  },
  { // CMD_BUFFS
    .name = "buffs",
    .command_id = TYPE_E_ACORN,
    .error_msg = "You cannot purchase an enchanted acorn with this embed! Please send `/buffs` to purchase them.",

    .func_cb = &buffs_interaction
  },
  { // CMD_INFO
    .name = "info",
    .command_id = TYPE_INFO,
    .error_msg = "This info embed belongs to someone else! Pkease send `/info` to view your own info.",
    
    .func_cb = &info_interaction
  },
  { // CMD_INFO_FROM_BUTTONS
    .name = "info",
    .command_id = TYPE_INFO_FROM_BUTTONS,
    .error_msg = "This info embed belongs to someone else! Pkease send `/info` to view your own info.",

    .func_cb = &info_from_buttons
  },
  { // CMD_COLOR
    .name = "color",
    .func_cb = &color_interaction
  },
  { // CMD_STEAL
    .name = "steal",
    .func_cb = &steal_interaction
  },
  { // CMD_SQUIRREL
    .name = "squirrels",
    .command_id = TYPE_SQUIRREL,
    .error_msg = "You cannot swap squirrels with this embed! Please send `/squirrels` to change your current squirrel.",

    .func_cb = &squirrels_interaction
  },
  { // CMD_LEADERBOARD
    .name = "leaderboard",
    .command_id = TYPE_LEADERBOARD,
    .error_msg = "This leaderboard belongs to someone else! Please send `/leaderboard` to interact.",

    .func_cb = &fetch_leaderboard
  },
  // EVENT COMMANDS
  { // CMD_EVENT_HELP
    .name = "event_help",
    .command_id = TYPE_EVENT_HELP,
    .error_msg = "This help embed belongs to someone else! Please send `/event_help` for support.",

    .func_cb = &event_help_interaction
  },
  { // CMD_SEASON_INFO
    .name = "season_info",
    .func_cb = &get_season_event
  },
  { // CMD_BUNNY_ENDEAVOR
    .name = "bunny_endeavor",
    .command_id = TYPE_BUNNY,
    .error_msg = "You cannot purchase an item off this embed! Please send `/bunny_endeavor` to make a purchase.",

    .func_cb = &bunny_interaction
  },
  // SCURRY COMMANDS
  { // CMD_SCURRY_HELP
    .name = "scurry_help",
    .command_id = TYPE_SCURRY_HELP,
    .error_msg = "This help embed belongs to someone else! Please send `/scurry_help` for support.",
    
    .func_cb = &scurry_help_interaction
  },
  { // CMD_SCURRY_INFO
    .name = "scurry_info",
    .command_id = TYPE_SCURRY_INFO,
    .error_msg = "You either do not have permission to press this button or this embed belongs to someone else!",

    .func_cb = &s_info_interaction
  },
  { // CMD_SCURRY_INVITE
    .name = "scurry_invite",
    .func_cb = &init_invite_interaction,
  },
  { // CMD_SCURRY_INVITE_RESP
    .name = "scurry_invite",
    .func_cb = &invite_interaction,
    // no match is needed since this is done through DM with the bot

    .command_id = TYPE_INVITE
  },
  { // CMD_SCURRY_KICK
    .name = "scurry_kick",
    .func_cb = &kick_interaction
  },
  { // CMD_SCURRY_LEAVE
    .name = "scurry_leave",
    .func_cb = &leave_interaction
  },
  { // CMD_SCURRY_CREATE
    .name = "scurry_create",
    .func_cb = &create_interaction
  }
};

void create_commands(struct discord *client, const struct discord_ready *event)
{
  (void)event;

  struct discord_create_global_application_command commands[] =
  {
    {
      .name = "biome_story",
      .description = "Background about the biomes.",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    // MAIN COMMANDS
    { // forage
      .name = "forage",
      .description = "Send to find some goodies! (Costs 2 energy)",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // upgrade
      .name = "upgrade",
      .description = "Upgrade your squirrel senses.",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // buffs
      .name = "buffs",
      .description = "Eat an enchanted acorn to enhance gameplay!",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // info
      .name = "info",
      .description = "View your player statistics.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
          .type = DISCORD_APPLICATION_OPTION_STRING,
          .name = "user",
          .description = "Mention the user or use their id",
          }
        },
        .size = 1
      }
    },
    { // color
      .name = "color",
      .description = "Change the color of your embed tab.",
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
    { // steal
      .name = "steal",
      .description = "Steal someone's acorns. (Costs 20 energy)",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // squirrels
      .name = "squirrels",
      .description = "Swap to a different squirrel!",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // vengeance_mode
      .name = "vengeance_mode",
      .description = "Enable or disable Vengeance Mode!",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },

    // HELP COMMANDS
    { // player_help
      .name = "player_help",
      .description = "Looking for help?",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // event_help
      .name = "event_help",
      .description = "Looking for help on events?",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // scurry_help
      .name = "scurry_help",
      .description = "Looking for help on scurries?",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },

    { // leaderboard
      .name = "leaderboard",
      .description = "View the acorn leaderboard! (Top 10)",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },

    // SCURRY COMMANDS
    { // scurry_info
      .name = "scurry_info",
      .description = "View your scurry information.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "scurry",
            .description = "Name the scurry",
          }
        },
        .size = 1
      }
    },
    { // scurry_invite
      .name = "scurry_invite",
      .description = "Request an invite using a scurry name.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "scurry",
            .description = "Name the scurry",
            .required = true
          }
        },
        .size = 1
      }
    },
    { // scurry_kick
      .name = "scurry_kick",
      .description = "Kick a member from your scurry.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "user",
            .description = "Mention the user",
            .required = true
          }
        },
        .size = 1
      }
    },
    { // scurry_leave
      .name = "scurry_leave",
      .description = "Leave your current scurry.",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "verify",
            .description = "Verify that you would like to leave this scurry by entering 'yes'. You can always rejoin!",
            .required = true
          }
        },
        .size = 1
      }
    },

    { // scurry_create
      .name = "scurry_create",
      .description = "Create a scurry for 50,000 acorns.",

      .type = DISCORD_APPLICATION_CHAT_INPUT,
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "name",
            .description = "What would you like to name your scurry? (Keep it SFW)",
            .required = true
          }
        },
        .size = 1
      }
    },

    // EVENT COMMANDS
    { // bunny_endeavor
      .name = "bunny_endeavor",
      .description = "Buy all sorts of resources with catnip (Only available during the in-game Winter!)",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // season_info
      .name = "season_info",
      .description = "Unsure of what the in-game season is?",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    }
  };

  for (int i = 0; i < (int)(sizeof(commands)/sizeof(*commands)); i++)
    discord_create_global_application_command(client, APPLICATION_ID, &commands[i], NULL);
}