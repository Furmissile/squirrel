struct sd_init_encounter {
  struct discord_component buttons[3];
  char custom_ids[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];

  char labels[3][64];

  char image_url[128];
  char description[512];
};

void init_encounter_buttons(const struct discord_interaction *event, struct sd_init_encounter *params, struct sd_player *player)
{
  struct sd_encounter encounter = biomes[player->biome].encounters[player->encounter];

  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                item_types[TYPE_ENCOUNTER].emoji_name),
        .id = item_types[TYPE_ENCOUNTER].emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_PRIMARY,
      .emoji = params->emojis,
      .label = u_snprintf(params->labels[button_idx], sizeof(params->labels[button_idx]), encounter.solutions[button_idx]),
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c%d_%ld",
                    TYPE_ENCOUNTER_RESP, button_idx, player->encounter + 96, player->biome, event->member->user->id)
    };
  }
}

int init_encounter_interaction(const struct discord_interaction *event, struct sd_player *player)
{
  struct sd_init_encounter params = { 0 };
  init_encounter_buttons(event, &params, player);
  
  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 3
    }
  };

  struct sd_header_params header = { 0 };
  struct sd_encounter encounter = biomes[player->biome].encounters[player->encounter];

  header.embed = (struct discord_embed) 
  {
    .color = player->color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },

    .title = u_snprintf(header.title, sizeof(header.title), encounter.name),
    .description = u_snprintf(params.description, sizeof(params.description), encounter.conflict),

    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player->biome].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          encounter.file_path)
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
        .array = &action_rows,
        .size = 1
      }
    }

  };

  char values[16384];
  CCORDcode code = discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}

struct sd_init_forage {
  struct discord_component buttons[3];
  char custom_ids[3][64];

  struct discord_emoji emojis[3];
  char emoji_names[3][64];

  char image_url[128];
};

void init_forage_buttons(const struct discord_interaction *event, struct sd_init_forage *params)
{
  for (int button_idx = 0; button_idx < 3; button_idx++) 
  {
    params->emojis[button_idx] = (struct discord_emoji) {
        .name = u_snprintf(params->emoji_names[button_idx], sizeof(params->emoji_names[button_idx]), 
                item_types[TYPE_DIRT_PILE].emoji_name),
        .id = item_types[TYPE_DIRT_PILE].emoji_id
    };

    params->buttons[button_idx] = (struct discord_component) 
    { 
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_PRIMARY,
      .emoji = params->emojis,
      .custom_id = u_snprintf(params->custom_ids[button_idx], sizeof(params->custom_ids[button_idx]), "%c%d%c_%ld",
                    TYPE_FORAGE_RESP, button_idx, ERROR_STATUS + 96, event->member->user->id)
    };
  }
}

int init_forage_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id); 

  energy_regen(&player);

  ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready! Please wait 2 seconds.");
  ERROR_INTERACTION((player.energy < 2), "You need more energy!");

  struct sd_init_forage params = { 0 };
  init_forage_buttons(event, &params);
  
  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 3
    }
  };

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Scouring the lands..."),
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          biomes[player.biome].biome_scene_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          item_types[TYPE_UNIDENTIFIED].file_path)
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
        .array = &action_rows,
        .size = 1
      }
    }

  };

  char values[16384];
  discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \n", values);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}
