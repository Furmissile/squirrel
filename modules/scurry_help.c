int scurry_help_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_help_info params = { 0 };

  int page_num = (player.button_idx != ERROR_STATUS) ? player.button_idx +1 : 1;

  switch (page_num -1)
  {
    case S_TOPIC_UTILS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""GUILD_ICON" Scurry Utils"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Member invites come in through DM. Plan accordingly because invites expire in **2** minutes! \n"
            " "BULLET" Only the "LEADER" *owner* can kick a member! Mention the user in "SCURRY_KICK_ID" to kick. \n"
            " "BULLET" Members can leave on their own prerogative too by pressing the \"LEAVE\" button at `Info > Scurry Info`. \n"
            "**Please know that your stats related to the scurry will not be retained!**")
      };
      break;
    case S_TOPIC_WARS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""GUILD_ICON" Scurry Wars"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" "WAR_ACORNS" *Total stolen acorn* is the scurry war currency that is obtained from stealing other scurries' war acorns. \n"
            " "BULLET" Any scurry currently in the war has the chance to steal from your "LOST_STASH" *war stash*! \n"
            " "BULLET" To join the arena found in `Info > Scurry Info`, scurries need their war stash full and have at least **3** members present!")
      };
      break;
    case S_TOPIC_AFTERMATH:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""GUILD_ICON" Arena Aftermath"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" Every scurry is dropped from the war upon running out of "WAR_ACORNS" *war acorns*. \n"
            " "BULLET" Every time you re-enter the war, your "WAR_ACORNS" *total stolen acorn* is reset. \n"
            " "BULLET" Scurries have the option to retreat at any point, but be aware your rank will match your new stolen acorn score! \n"
            " "BULLET" Scurry ranks provide passive "ACORNS" *acorn* boosts!")
      };
      break;
    case S_TOPIC_RANKS:
      APPLY_NUM_STR(seed_not_max, SEED_NOT_MAX);
      APPLY_NUM_STR(acorn_snatcher_max, ACORN_SNATCHER_MAX);
      APPLY_NUM_STR(seed_sniffer_max, SEED_SNIFFER_MAX);
      APPLY_NUM_STR(oakfficial_max, OAKFFICIAL_MAX);
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""GUILD_ICON" War Ranks"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "BULLET" "WAR_ACORNS" *Stolen acorns* determines scurry rank as follows: \n"
            " "INDENT" ("ACORNS") Seed-Nots (*Base*) \n"
            " "INDENT" ("ACORNS") Acorn Snatchers (> **%s** "WAR_ACORNS" *Stolen Acorns*): x**1.05** \n"
            " "INDENT" ("GOLDEN_ACORNS") Seed Sniffers (> **%s** "WAR_ACORNS" *Stolen Acorns*) x**1.1** \n"
            " "INDENT" ("GOLDEN_ACORNS") Oakfficials (> **%s** "WAR_ACORNS" *Stolen Acorns*) x**1.15** \n"
            " "INDENT" ("CONJURED_ACORNS") Royal Nuts (**%s**+ "WAR_ACORNS" *Stolen Acorns*) x**1.2**",
            seed_not_max, acorn_snatcher_max, seed_sniffer_max, oakfficial_max)
      };
      break;
  }

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Scurry Help (%d of %d)", 
        page_num, S_TOPIC_SIZE),

    .fields = &(struct discord_embed_fields) {
      .array = &params.field,
      .size = 1
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ACORNS].file_path)
    }
  };

  struct sd_pages pages = { 0 };

  init_page_buttons(event, &pages, page_num -1, TYPE_SCURRY_HELP, S_TOPIC_SIZE -1);

  struct sd_help_buttons help = { 0 };

  init_help_buttons(event, &help, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = pages.buttons,
        .size = sizeof(pages.buttons)/sizeof(*pages.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = help.buttons,
        .size = sizeof(help.buttons)/sizeof(*help.buttons)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = sizeof(util_data.buttons)/sizeof(*util_data.buttons)
      }
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = action_rows,
        .size = 3
      }
    }

  };

  char values[16384];
  CCORDcode code = discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}