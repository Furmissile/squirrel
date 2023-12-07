struct sd_session_info 
{
  struct discord_embed_field fields[2];
  char field_names[2][64];
  char field_values[2][1024];

  char description[512];

  char footer_text[64];
  char footer_url[128];
};

int session_info_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_header_params header = { 0 };
  struct sd_session_info params = { 0 };

  APPLY_NUM_STR(acorn_count, player.session_data.acorn_count);
  APPLY_NUM_STR(acorns, player.session_data.acorns);
  APPLY_NUM_STR(golden_acorns, player.session_data.golden_acorns);
  APPLY_NUM_STR(health_loss, player.session_data.health_loss);

  params.fields[0].name = u_snprintf(params.field_names[0], sizeof(params.field_names[0]), "Session Resources");

  params.fields[0].value = u_snprintf(params.field_values[0], sizeof(params.field_values[0]), 
      " "INDENT" "ACORN_COUNT" Acorn Count: **%s** \n"
      " "INDENT" "ACORNS" Acorns: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Net Golden Acorns: **%s** \n"
      " "INDENT" "BROKEN_HEALTH" Health Loss: **%s**",
      acorn_count, acorns, golden_acorns, health_loss);

  APPLY_NUM_STR(no_acorns, player.session_data.no_acorns);
  APPLY_NUM_STR(acorn_handful, player.session_data.acorn_handful);
  APPLY_NUM_STR(acorn_mouthful, player.session_data.acorn_mouthful);
  APPLY_NUM_STR(lost_stash, player.session_data.lost_stash);
  APPLY_NUM_STR(acorn_sack, player.session_data.acorn_sack);
  APPLY_NUM_STR(total_forages, player.session_data.total_forages);

  params.fields[1].name = u_snprintf(params.field_names[1], sizeof(params.field_names[1]), "Session Rewards");
  u_snprintf(params.field_values[1], sizeof(params.field_values[1]), 
      " "INDENT" "NO_ACORNS" No Acorns: **%s** \n"
      " "INDENT" "ACORN_HANDFUL" Acorn Handful: **%s** \n"
      " "INDENT" "ACORN_MOUTHFUL" Acorn Mouthful: **%s** \n"
      " "INDENT" "LOST_STASH" Lost Stash: **%s** \n"
      " "INDENT" "ACORN_SACK" Acorn Sack: **%s** \n",
      no_acorns, acorn_handful, acorn_mouthful, lost_stash, acorn_sack);

  // time sensitive stats
  struct tm *info = get_UTC();
  if (info->tm_mon == CHRISTMAS_MONTH)
  {
    APPLY_NUM_STR(ribboned_acorns, player.session_data.ribboned_acorns);

    u_snprintf(params.field_values[1], sizeof(params.field_values[1]),
        " "INDENT" "RIBBONED_ACORN" Ribboned Acorns: **%s** \n", 
        ribboned_acorns);
  }

  u_snprintf(params.field_values[1], sizeof(params.field_values[1]), "\n "STAHR" Total: **%s**", total_forages);

  params.fields[1].value = params.field_values[1];

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Session Info"),
    .description = u_snprintf(params.description, sizeof(params.description), 
        ""OFF_ARROW" The info below is based on *forages* done in a session. \n"
        ""OFF_ARROW" A session lasts until **2** minutes has gone by without any activity. \n"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          (player.designer_squirrel == ERROR_STATUS) 
            ? squirrels[player.squirrel].squirrel.file_path 
            : designer_squirrels[player.designer_squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 2
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Welcome to Session Info!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path)
    }
  };

  struct sd_statistics stats = { 0 };

  init_statistics_buttons(event, &stats, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  struct discord_component action_rows[2] = 
  {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = stats.buttons,
        .size = sizeof(stats.buttons)/sizeof(*stats.buttons)
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
    .type = DISCORD_INTERACTION_UPDATE_MESSAGE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = action_rows,
        .size = 2
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}