struct sd_start
{
  struct discord_component button;
  char custom_id[64];
  char label[64];

  struct discord_emoji emoji;
  char emoji_name[64];

  char image_url[128];

  char footer_text[64];
  char footer_url[128];
};

int init_welcome_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id, event->data->custom_id);

  struct sd_header_params header = { 0 };
  struct sd_start params = { 0 };

  struct sd_file_data squirrel_dir = (player.designer_squirrel == ERROR_STATUS) 
      ? squirrels[player.squirrel].squirrel
      : designer_squirrels[player.designer_squirrel].squirrel;

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Welcome to Squirrel Dash!"),
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          misc[MISC_WELCOME].file_path)
    },
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrel_dir.file_path)
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ACORNS].file_path)
    }
  };

  params.emoji = (struct discord_emoji)
  {
    .name = u_snprintf(params.emoji_name, sizeof(params.emoji_name), 
        items[ITEM_ACORNS].emoji_name),
    .id = items[ITEM_ACORNS].emoji_id
  };
  params.button = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .custom_id = u_snprintf(params.custom_id, sizeof(params.custom_id), "%c0.%ld",
        TYPE_INFO_FROM_BUTTONS, event->member->user->id),
    .label = u_snprintf(params.label, sizeof(params.label), "Start!"),
    .emoji = &params.emoji
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = &params.button,
      .size = 1
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

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

  update_player_row(&player);

  return 0;
}