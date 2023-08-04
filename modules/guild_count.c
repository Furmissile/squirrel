void inc_count(
  struct discord *client, 
  const struct discord_guild *event) 
{
  (void)event;
  guild_count++;

  char msg_content[256] = { };

  discord_modify_channel(client, GUILD_COUNT_CHANNEL,
    &(struct discord_modify_channel) {
      .name = u_snprintf(msg_content, sizeof(msg_content), "Guild Count: %d", guild_count),
      .reason = "update guild count",
      .type = DISCORD_CHANNEL_GUILD_VOICE,
      .rtc_region = "us-east"
    },
    NULL);

  printf("Guild count: %d \n", guild_count);
}

void dec_count(
  struct discord *client, 
  const struct discord_guild *event) 
{
  (void)event;
  guild_count--;

  char msg_content[256] = { };

  discord_modify_channel(client, GUILD_COUNT_CHANNEL,
    &(struct discord_modify_channel) {
      .name = u_snprintf(msg_content, sizeof(msg_content), "Guild Count: %d", guild_count),
      .reason = "update guild count",
      .type = DISCORD_CHANNEL_GUILD_VOICE,
      .rtc_region = "us-east"
    },
    NULL);

  printf("Guild count: %d \n", guild_count);
}

void init_guild_count(struct discord *client, const struct discord_ready *event)
{
  guild_count = event->guilds->size;

  char msg_content[256] = { };

  discord_modify_channel(client, GUILD_COUNT_CHANNEL,
    &(struct discord_modify_channel) {
      .name = u_snprintf(msg_content, sizeof(msg_content), "Guild Count: %d", guild_count),
      .reason = "update guild count",
      .type = DISCORD_CHANNEL_GUILD_VOICE,
      .rtc_region = "us-east"
    },
    NULL);
}