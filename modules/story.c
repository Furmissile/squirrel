int biome_story_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  ERROR_INTERACTION((event->message->timestamp /1000 < player.timestamp), 
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp /1000;

  if (APPLICATION_ID == MAIN_BOT_ID)
    ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready! Please wait %d second(s).", BASE_CD);

  player.main_cd = time(NULL) + BASE_CD;

  struct sd_biome_info params = { 0 };

  int page_num = (player.button_idx != ERROR_STATUS) ? player.button_idx +1 : 1;

  int field_count = biomes[page_num -1].biome_size;
  params.fields = calloc(field_count, sizeof(struct discord_embed_field));
  params.field_names = calloc(field_count, sizeof(*params.field_names));
  params.field_values = calloc(field_count, sizeof(*params.field_values));

  if (page_num -1 < BIOME_SIZE)
  {
    int total_encounter_size = 0;

    // for each biome section
    for (int section_idx = 0; section_idx < field_count; section_idx++)
    {
      params.fields[section_idx].name = u_snprintf(params.field_names[section_idx], sizeof(params.field_names[section_idx]), "%s", 
          biomes[page_num -1].sections[section_idx].section_name);

      // for each encounter...
      for (int encounter_idx = 0; encounter_idx < biomes[page_num -1].sections[section_idx].section_size; encounter_idx++)
      {
        struct sd_encounter encounter = biomes[page_num -1].sections[section_idx].encounters[encounter_idx];

        int* biome_stories[5] = { 
          &player.story.grasslands, 
          &player.story.seeping_sands, 
          &player.story.nature_end, 
          &player.story.death_grip, 
          &player.story.last_acorn 
        };

        if ((*biome_stories[page_num -1] >> total_encounter_size & 0x01) == 1)
        {
          // this encounter has been seen: print out info
          u_snprintf(params.field_values[section_idx], sizeof(params.field_values[section_idx]), " "BULLET" \"*%s*\" \n", encounter.context);
        }
        else {
          // this encounter hasnt been seen: print out "Complete "name" to see this info!"
          u_snprintf(params.field_values[section_idx], sizeof(params.field_values[section_idx]), 
              " "QUEST_MARKER" || Complete \"%s\" to view element. || \n",
              encounter.name);
        }

        params.fields[section_idx].value = params.field_values[section_idx];

        total_encounter_size++;
      }

    }

  }

  struct sd_header_params header = { 0 };

  char image_url[128] = { };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Biomes (%d of %d)", 
        page_num, STORY_TOPIC_SIZE),
    .description = u_snprintf(params.description, sizeof(params.description), "%s", biomes[page_num -1].intro),

    .image = &(struct discord_embed_image) {
      .url = u_snprintf(image_url, sizeof(image_url), GIT_PATH, biomes[page_num -1].biome_scene_path)
    },

    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = field_count
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, slice_types[TYPE_ACORNS].item.file_path)
    }
  };

  struct sd_pages pages = { 0 };

  init_page_buttons(event, &pages, page_num -1, TYPE_BIOME_STORY, STORY_TOPIC_SIZE -1);

  struct sd_statistics stats = { 0 };

  init_statistics_buttons(event, &stats, &player);

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
        .array = util_data.buttons,
        .size = util_data.buttons_displayed
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = stats.buttons,
        .size = sizeof(stats.buttons)/sizeof(*stats.buttons)
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

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  update_player_row(&player);

  free(params.fields);
  free(params.field_names);
  free(params.field_values);

  return 0;
}