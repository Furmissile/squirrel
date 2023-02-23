struct sd_command
{
  char* name;
  char command_id; // custom id prefix (if one exists)
  char* error_msg;

  int (*func_cb)(const struct discord_interaction *event, struct sd_message *discord_msg);
};

enum CMD_TYPE
{
  CMD_MAIN_FORAGE,
  CMD_ENCOUNTER_FORAGE,
  CMD_UPGRADE,
  CMD_BUFFS,
  CMD_INFO,
  CMD_COLOR,
  CMD_STEAL,
  CMD_SQUIRREL,

  CMD_SEASON_INFO,
  CMD_BUNNY_ENDEAVOR,

  CMD_PLAYER_HELP,
  CMD_EVENT_HELP,
  CMD_SCURRY_HELP,

  CMD_LEADERBOARD,

  CMD_SCURRY_INFO,
  CMD_SCURRY_INVITE,
  CMD_SCURRY_KICK,
  CMD_SCURRY_LEAVE,
  CMD_SCURRY_CREATE,

  CMD_UTIL_RESOURCES,
  CMD_UTIL_STATS,
  CMD_UTIL_BUFFS,
  CMD_UTIL_SCURRY_RES,
  CMD_UTIL_DELETE_PLAYER,
  CMD_UTIL_DELETE_SCURRY,
  CMD_UTIL_RULES,
  CMD_UTIL_VERIFY,
  // CMD_UTIL_CREATE_BETA,
  CMD_SIZE
};

static struct sd_command *cmds = (struct sd_command[])
{
  // MAIN COMMANDS
  { // CMD_MAIN_FORAGE
    .name = "forage", // can be skipped when comparing names
    .command_id = TYPE_ENCOUNTER_MSG,
    .error_msg = "This is someone else's adventure! Please send `/forage` to start your own adventure!",

    .func_cb = &forage_interaction
  },
  { // CMD_ENCOUNTER_FORAGE
    .name = "forage",
    .command_id = TYPE_MAIN_MSG,
    .error_msg = "This is someone else's adventure! Please send `/forage` to start your own adventure!",

    .func_cb = &forage_interaction
  },
  { // CMD_UPGRADE
    .name = "upgrade",
    .command_id = TYPE_UPGRADE,
    .error_msg = "You cannot upgrade your stats with this embed! Please send `/upgrade` to access upgrades.",

    .func_cb = &store_interaction
  },
  { // CMD_BUFFS
    .name = "buffs",
    .command_id = TYPE_E_ACORN,
    .error_msg = "You cannot purchase an enchanted acorn with this embed! Please send `/buffs` to purchase them.",

    .func_cb = &buffs_interaction
  },
  { // CMD_INFO
    .name = "info",
    .func_cb = &info_interaction
  },
  { // CMD_COLOR
    .name = "color",
    .func_cb = &color_interaction
  },
  { // CMD_STEAL
    .name = "steal",
    .func_cb = &steal_interaction
  },
  {
    .name = "squirrels",
    .command_id = TYPE_SQUIRREL,
    .error_msg = "You cannot swap squirrels with this embed! Please send `/squirrels` to change your current squirrel.",

    .func_cb = &squirrels_interaction
  },

  // HELP COMMANDS
  { // CMD_PLAYER_HELP
    .name = "player_help",
    .func_cb = &help_interaction
  },
  { // CMD_EVENT_HELP
    .name = "event_help",
    .func_cb = &event_help_interaction
  },
  { // CMD_SCURRY_HELP
    .name = "scurry_help",
    .func_cb = &scurry_help_interaction
  },

  { // CMD_LEADERBOARD
    .name = "leaderboard",
    .func_cb = &get_leaderboard
  },

  // EVENT COMMANDS
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
  { // CMD_SCURRY_INFO
    .name = "scurry_info",
    .command_id = TYPE_SCURRY_WAR,
    .error_msg = "You are not part of this guild!",

    .func_cb = &s_info_interaction
  },
  { // CMD_SCURRY_INVITE
    .name = "scurry_invite",
    .func_cb = &invite_interaction,
    // no match is needed since this is done through DM with the bot

    .command_id = TYPE_INVITE,
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
  },

  // UTIL COMMANDS...
  { // CMD_UTIL_RESOURCES
    .name = "util_set_resources",
    .func_cb = &set_resources_interaction,
  },
  { // CMD_UTIL_STATS
    .name = "util_set_stats",
    .func_cb = &set_stats_interaction,
  },
  { // CMD_UTIL_BUFFS
    .name = "util_set_buffs",
    .func_cb = &set_buffs_interaction
  },
  { // CMD_UTIL_SCURRY_RES
    .name = "util_set_scurry_res",
    .func_cb = &set_scurry_res_interaction
  },
  { // CMD_UTIL_DELETE_PLAYER
    .name = "util_delete_player",
    .func_cb = &delete_player_interaction
  },
  { // CMD_UTIL_DELETE_SCURRY
    .name = "util_delete_scurry",
    .func_cb = &delete_scurry_interaction
  },
  { // CMD_UTIL_RULES
    .name = "util_rules",
    .func_cb = &rules_interaction
  },
  { // CMD_UTIL_VERIFY
    .name = "util_verify",
    .func_cb = &create_verify_interaction
  },
  // { // CMD_UTIL_CREATE_BETA
  //   .name = "util_create_beta",
  //   .func_cb = &beta_interaction
  // }
};

void create_util_commands(struct discord *client, const struct discord_guild *guild)
{
  struct discord_create_guild_application_command commands[] =
  {
    // UTIL COMMANDS
    { //set_resources
      .name = "util_set_resources",
      .description = "Set a player resources",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_user",
            .description = "User mention or id that will receive the resources",
            .required = true
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_ACORNS].item.emoji_name,
            .description = "Set player acorns"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_ACORN_COUNT].item.emoji_name,
            .description = "Set player passive acorns"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_GOLDEN_ACORN].item.emoji_name,
            .description = "Set player golden acorns"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_STOLEN_ACORNS].item.emoji_name,
            .description = "Set player stolen acorns"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_ENERGY].item.emoji_name,
            .description = "Set player energy"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_HEALTH].item.emoji_name,
            .description = "Set player health"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = items[ITEM_CATNIP].item.emoji_name,
            .description = "Set player catnip"
          }
        },

        .size = ITEM_SIZE +1
      }
    },
    { // util_set_stats
      .name = "util_set_stats",
      .description = "Set a player stat",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_user",
            .description = "User mention or id that will receive the resources",
            .required = true
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = stats[STAT_PROFICIENCY].stat.emoji_name,
            .description = "Set player proficiency"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = stats[STAT_LUCK].stat.emoji_name,
            .description = "Set player luck"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = stats[STAT_STRENGTH].stat.emoji_name,
            .description = "Set player strength"
          }
        },

        .size = STAT_SIZE +1
      }
    },
    { // util_set_buffs
      .name = "util_set_buffs",
      .description = "Set a player buffs",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_user",
            .description = "User mention or id that will receive the resources",
            .required = true
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = enchanted_acorns[BUFF_PROFICIENCY_ACORN].item.emoji_name,
            .description = "Set player proficiency acorn"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = enchanted_acorns[BUFF_LUCK_ACORN].item.emoji_name,
            .description = "Set player luck acorn"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = enchanted_acorns[BUFF_STRENGTH_ACORN].item.emoji_name,
            .description = "Set player strength acorn"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = enchanted_acorns[BUFF_ENDURANCE_ACORN].item.emoji_name,
            .description = "Set player strength acorn"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = enchanted_acorns[BUFF_DEFENSE_ACORN].item.emoji_name,
            .description = "Set player strength acorn"
          }
        },

        .size = BUFFS_SIZE +1
      }
    },
    { // util_set_scurry_res
      .name = "util_set_scurry_res",
      .description = "Set a scurry's resources",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_scurry",
            .description = "Scurry owner id that will receive the resources",
            .required = true
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = scurry_items[SCURRY_ITEM_COURAGE].item.emoji_name,
            .description = "Set scurry's courage"
          },
          {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = scurry_items[SCURRY_ITEM_WAR_ACORNS].item.emoji_name,
            .description = "Set scurry's war acorns"
          }
        },

        .size = SCURRY_ITEM_SIZE +1
      }
    },
    { // util_delete_player
      .name = "util_delete_player",
      .description = "Reset a player's stats",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_user",
            .description = "Name of user that will be reset",
            .required = true
          }
        },

        .size = 1
      }
    },
    { // util_delete_scurry
      .name = "util_delete_scurry",
      .description = "Delete a scurry",
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "target_scurry",
            .description = "Name of scurry that will be deleted",
            .required = true
          }
        },

        .size = 1
      }
    },
    { // util_rules
      .name = "util_rules",
      .description = "Load the rules embed",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // util_verify
      .name = "util_verify",
      .description = "Load the verify embed",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // util_create_beta
      .name = "util_create_beta",
      .description = "Refresh a new beta table for testing",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    }
  };

  for (int i = 0; i < (int)(sizeof(commands)/sizeof(*commands)); i++)
    discord_create_guild_application_command(client, APPLICATION_ID, guild->id, &commands[i], NULL);
}

void create_commands(struct discord *client, const struct discord_guild *guild)
{
  struct discord_create_guild_application_command commands[] =
  {
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
      .description = format_str(SIZEOF_DESCRIPTION, 
          "Change the color of your embed tab for %s acorns.", 
          num_str(COLOR_COST)),
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
      .description = format_str(SIZEOF_DESCRIPTION, 
          "Steal someone's acorns. (Costs %s energy)", 
          num_str(STEAL_ENERGY_COST)),
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // squirrels
      .name = "squirrels",
      .description = format_str(SIZEOF_DESCRIPTION,
          "Swap to a different squirrel!"),
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
      .type = DISCORD_APPLICATION_CHAT_INPUT,

      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "type",
            .description = "Select the type of leaderboard",
            
            .choices = &(struct discord_application_command_option_choices)
            {
              .array = (struct discord_application_command_option_choice[])
              {
                {
                    .name = "acorn_count",
                    .value = "\"acorn_count\"",
                },
                {
                    .name = "courage",
                    .value = "\"courage\"",
                }
              },
              .size = 2
            },
          }
        },
        .size = 1
      }
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
      .description = format_str(SIZEOF_DESCRIPTION, 
          "Create a scurry for %s acorns.", 
          num_str(SCURRY_CREATION_COST) ),

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
    discord_create_guild_application_command(client, APPLICATION_ID, guild->id, &commands[i], NULL);

  // must be parent server and be Future Squirrel to initialize util commands!!!
  if (guild->id == GUILD_ID)
    create_util_commands(client, guild);
}
