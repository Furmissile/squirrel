struct sd_daily_tasks
{
  struct discord_component buttons[3 +1]; // +1 for refresh button!
  char custom_ids[3 +1][64];
  char labels[3 +1][64];

  struct discord_emoji emoji;
  char emoji_name[64];

  struct discord_embed_field fields[3 +2]; // +2 offset for rewards and time left field
  char field_names[3 +2][128];
  char field_values[3 +2][256];

  char description[512];

  char footer_text[64];
  char footer_url[128];

  int unit_acorn;
  int unit_golden_acorn;
};

void init_daily_tasks_fields(struct sd_daily_tasks *params, struct sd_player *player, struct tm *info)
{
  int* task_counts[3] = {
    &player->daily.primary,
    &player->daily.secondary,
    &player->daily.tertiary
  };

  int* tasks_complete[3] = {
    &player->daily.primary_complete,
    &player->daily.secondary_complete,
    &player->daily.tertiary_complete
  };

  APPLY_NUM_STR(unit_acorn,  params->unit_acorn)
  APPLY_NUM_STR(unit_golden_acorn, params->unit_golden_acorn);

  // rewards field
  params->fields[0] = (struct discord_embed_field)
  {
    .name = u_snprintf(params->field_names[0], sizeof(params->field_names[0]), "Task Reward"),
    .value = u_snprintf(params->field_values[0], sizeof(params->field_values[0]),
        " "INDENT" **%s** "ACORNS" Acorns \n"
        " "INDENT" **%s** "GOLDEN_ACORNS" Golden Acorns \n", 
        unit_acorn, unit_golden_acorn)
  };

  // time field
  params->fields[1] = (struct discord_embed_field) { 0 };

  params->fields[1].name = u_snprintf(params->field_names[1], sizeof(params->field_names[1]), "Time Left");

  int time_left = 24 - info->tm_hour;

  if (time_left >= 1)
  {
    params->fields[1].value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]),
        " "INDENT" **%d** Hours \n", 
        24 - info->tm_hour);
  }
  else {
    params->fields[1].value = u_snprintf(params->field_values[1], sizeof(params->field_values[1]), " "INDENT" <**1** Hour \n");
  }

  int current_task = 0;

  // task progress fields
  for (int task_idx = 0; task_idx < TASK_SIZE; task_idx++)
  {
    if ((player->daily.active_tasks >> task_idx & 1) == 1)
    {
      int current_field = current_task +2;

      // if task is not complete: display description and progress
      if (*tasks_complete[current_task] == 1)
      {
        params->fields[current_field] = (struct discord_embed_field)
        {
          .name = u_snprintf(params->field_names[current_field], sizeof(params->field_names[current_field]), 
              "%s (%d/%d)", 
              daily_tasks[task_idx].task_name, daily_tasks[task_idx].count_req, daily_tasks[task_idx].count_req),

          .value = u_snprintf(params->field_values[current_field], sizeof(params->field_values[current_field]),
              " "BULLET" %s \n"
              " "QUEST_MARKER" *Complete*!",
              daily_tasks[task_idx].description)
        };
      }
      else if (*tasks_complete[current_task] == ERROR_STATUS)
      {
        params->fields[current_field] = (struct discord_embed_field)
        {
          .name = u_snprintf(params->field_names[current_field], sizeof(params->field_names[current_field]), 
              "%s", daily_tasks[task_idx].task_name),
          .value = u_snprintf(params->field_values[current_field], sizeof(params->field_values[current_field]),
              " "QUEST_MARKER" *Complete*!")
        };
      }
      else {
        params->fields[current_field] = (struct discord_embed_field)
        {
          .name = u_snprintf(params->field_names[current_field], sizeof(params->field_names[current_field]), 
              "%s (%d/%d)", 
              daily_tasks[task_idx].task_name, *task_counts[current_task], daily_tasks[task_idx].count_req),

          .value = u_snprintf(params->field_values[current_field], sizeof(params->field_values[current_field]),
              " "BULLET" %s", daily_tasks[task_idx].description)
        };
      }

      current_task++;
    }
  }
}

void init_daily_tasks_buttons(struct sd_daily_tasks *params, struct sd_player *player)
{
  int* tasks_complete[3] = {
    &player->daily.primary_complete,
    &player->daily.secondary_complete,
    &player->daily.tertiary_complete
  };

  // when ready to claim tasks!
  params->emoji = (struct discord_emoji)
  {
    .name = item_types[TYPE_ENCOUNTER].emoji_name,
    .id = item_types[TYPE_ENCOUNTER].emoji_id
  };

  for (int button_idx = 0; button_idx < 3; button_idx++)
  {
    params->buttons[button_idx] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d.%ld",
        TYPE_DAILY, button_idx, player->user_id)
    };

    if (*tasks_complete[button_idx] == 1)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SUCCESS;
      params->buttons[button_idx].label = "Claim!";
      params->buttons[button_idx].emoji = &params->emoji;
    }
    else if (*tasks_complete[button_idx] == ERROR_STATUS)
    {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
      params->buttons[button_idx].label = "claimed";
    }
    else {
      params->buttons[button_idx].style = DISCORD_BUTTON_SECONDARY;
      params->buttons[button_idx].disabled = true;
      params->buttons[button_idx].label = "in-progress...";
    }
  }
}

void daily_tasks_cmd_state(struct sd_daily_tasks *params, struct sd_player *player)
{
  if (player->button_idx < 3)
  {
    int* tasks_complete[3] = {
      &player->daily.primary_complete,
      &player->daily.secondary_complete,
      &player->daily.tertiary_complete
    };

    int button_idx = player->button_idx;
    if (*tasks_complete[button_idx] == ERROR_STATUS)
    {
      u_snprintf(params->footer_text, sizeof(params->footer_text), "You have already claimed this reward!");
      u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
    }
    else {
      int current_task = 0;

      for (int task_idx = 0; task_idx < TASK_SIZE; task_idx++)
      {
        if ((player->daily.active_tasks >> task_idx & 1) == 1)
        {
          if (button_idx == current_task)
          {
            player->acorns += params->unit_acorn;
            player->golden_acorns += params->unit_golden_acorn;
            *tasks_complete[button_idx] = ERROR_STATUS;

            u_snprintf(params->footer_text, sizeof(params->footer_text), "%s reward claimed!", daily_tasks[task_idx].task_name);
            u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, items[ITEM_ACORNS].file_path);
            break;
          }

          current_task++;
        }
      }
      // update player only on selection
      update_player_row(player);
    }
  }
  else {
    u_snprintf(params->footer_text, sizeof(params->footer_text), "Welcome to Daily Tasks!");
    u_snprintf(params->footer_url, sizeof(params->footer_url), GIT_PATH, item_types[TYPE_ENCOUNTER].file_path);
  }
}

int daily_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  daily_task_progression(event, &player, ERROR_STATUS);

  struct sd_header_params header = { 0 };
  struct sd_daily_tasks params = { 0 };

  int proficiency_price = generate_price(player.stats.proficiency_lv, PROFICIENCY_UNIT);

  params.unit_acorn = proficiency_price /3; // completing dailies will earn at least 1 inc in proficiency!
  int highest_acorn_count = (player.high_acorn_count == 0) ? player.acorn_count : player.high_acorn_count;
  params.unit_golden_acorn = BIOME_ENCOUNTER_COST * (highest_acorn_count/BIOME_INTERVAL +1) *2;

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
  struct tm *info = get_UTC();

  daily_tasks_cmd_state(&params, &player);

  init_daily_tasks_fields(&params, &player, info);
  init_daily_tasks_buttons(&params, &player);

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Daily Tasks (%s %d)",
        months[info->tm_mon], info->tm_mday),
    .description = u_snprintf(params.description, sizeof(params.description), 
        " "OFF_ARROW" The info below displays your current progress with daily tasks. \n"
        " "OFF_ARROW" The rewards shown below are given per task completed. \n"),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          (player.designer_squirrel == ERROR_STATUS) 
            ? squirrels[player.squirrel].squirrel.file_path 
            : designer_squirrels[player.designer_squirrel].squirrel.file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = 3 +2
    },
    .footer = &(struct discord_embed_footer) {
      .text = params.footer_text,
      .icon_url = params.footer_url
    }
  };

  struct sd_secondary secondary = { 0 };

  generate_secondary_buttons(event, &secondary, &player);

  struct sd_util_info util_data = { 0 };

  generate_util_buttons(event, &player, &util_data);

  params.buttons[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .custom_id = u_snprintf(params.custom_ids[3], sizeof(params.custom_ids[3]), "%c3.%ld",
      TYPE_DAILY, event->member->user->id),
    .label = "Refresh"
  };

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = params.buttons,
        .size = 4
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = secondary.buttons,
        .size = secondary.button_size
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_data.buttons,
        .size = 5
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
      .components = &(struct discord_components)
      {
        .array = action_rows,
        .size = 3
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);
  
  return 0;
}