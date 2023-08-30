
void set_reaction(struct discord *client, struct discord_response *resp, const struct discord_message *message)
{
  (void)resp;
  discord_create_reaction(client, VERIFY_CHANNEL_ID, message->id, items[ITEM_ACORNS].emoji_id, items[ITEM_ACORNS].emoji_name, NULL);
}

void create_verify(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  (void)resp;

  struct sd_header_params header = { 0 };

  char msg_description[256] = { };

  header.embed = (struct discord_embed) 
  {
    .color = (int)strtol("0EDF0A", NULL, 16),
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          user->id, user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Please Verify"),
    .description = u_snprintf(msg_description, sizeof(msg_description), 
        "By reacting to this message, you have read and agree to the server rules. \n"
        "Please react with the Acorn "ACORNS" emoji to be able to chat. No other emoji will work!"),

    .thumbnail = &(struct discord_embed_thumbnail) {
      .url = u_snprintf(header.thumbnail_url, sizeof(header.thumbnail_url), GIT_PATH, VERIFY_PLUS_PATH)
    }
  };

  struct discord_create_message verify_msg = (struct discord_create_message) {
    .embeds = &(struct discord_embeds) {
      .array = &header.embed,
      .size = 1
    }
  };

  struct discord_ret_message ret_msg = { .done = &set_reaction };

  discord_create_message(client, VERIFY_CHANNEL_ID, &verify_msg, &ret_msg);
}

void verify_interaction(struct discord *client, const struct discord_message *event) 
{
  if (event->author->bot || event->channel_id != VERIFY_CHANNEL_ID)
    return;

  struct discord_ret_user ret_bot = { .done = &create_verify };

  // always use the production bot's ID
  discord_get_user(client, 905163158149287936, &ret_bot);
}