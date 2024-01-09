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

  if (APPLICATION_ID == MAIN_BOT_ID)
    ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready! Please wait %d second(s).", BASE_CD);
  
  player.main_cd = time(NULL) + BASE_CD;

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Welcome, %s!", event->member->user->username),
    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH,
          squirrels[player.squirrel].squirrel.file_path)
    },
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          misc[MISC_WELCOME].file_path)
    }
  };

  params.emoji = (struct discord_emoji)
  {
    .name = u_snprintf(params.emoji_name, sizeof(params.emoji_name), 
        slice_types[TYPE_ACORNS].item.emoji_name),
    .id = slice_types[TYPE_ACORNS].item.emoji_id,
    .animated = true
  };
  params.button = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .custom_id = u_snprintf(params.custom_id, sizeof(params.custom_id), "%c.%ld",
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

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}
