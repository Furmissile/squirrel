struct sd_rules 
{
  char description[256];
  char image_url[128];
  char footer_txt[64];
  char footer_url[128];

  struct discord_embed_field fields[RULE_SIZE];
  char field_names[RULE_SIZE][64];
  char field_values[RULE_SIZE][512];
};

void create_rules(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  (void)resp;

  struct sd_rules params = { 0 };

  params.fields[RULE_RESPECT] = (struct discord_embed_field)
  {
    .name = u_snprintf(params.field_names[RULE_RESPECT], sizeof(params.field_names[RULE_RESPECT]), ""ACORNS" Respect"),
    .value = u_snprintf(params.field_values[RULE_RESPECT], sizeof(params.field_values[RULE_RESPECT]),
        "*Please treat others with respect and be mindful of the things you say.* \n"
        ""BULLET" Hate speech, violent or obscene comments, and otherwise inappropriate or invasive behavior is not tolerated. \n"
        ""BULLET" For reference, use common sense!")
  };

  params.fields[RULE_CONTENT] = (struct discord_embed_field)
  {
    .name = u_snprintf(params.field_names[RULE_CONTENT], sizeof(params.field_names[RULE_CONTENT]), ""ACORNS" Content"),
    .value = u_snprintf(params.field_values[RULE_CONTENT], sizeof(params.field_values[RULE_CONTENT]),
        "*Make sure your content is posted in the right channel and **avoid** the following*: \n"
        " "BULLET" Harmful links or malware. If you don't know the link, don't click it! \n"
        " "BULLET" Controversial topics such as politcs, religion, etc. (or contributing to) \n"
        " "BULLET" Spamming pings or mentions, messages, etc. \n"
        " "BULLET" Adult content (NSFW). **If a minor shouldn't see it, then don't post it!**")
  };

  params.fields[RULE_BOT] = (struct discord_embed_field)
  {
    .name = u_snprintf(params.field_names[RULE_BOT], sizeof(params.field_names[RULE_BOT]), ""ACORNS" Bot Policy"),
    .value = u_snprintf(params.field_values[RULE_BOT], sizeof(params.field_values[RULE_BOT]),
        " "BULLET" There is a license on the bot discussing use of content. Visit the repo and review the license [**here**](https://github.com/Furmissile/squirrel/blob/main/LICENSE). \n"
        " "BULLET" If you have any questions regarding the license, please reach out using <#1047233819201261748> or by DMing <@582648847881338961>!")
  };

  params.fields[RULE_ADVERTISING] = (struct discord_embed_field)
  {
    .name = u_snprintf(params.field_names[RULE_ADVERTISING], sizeof(params.field_names[RULE_ADVERTISING]), ""ACORNS" Advertising"),
    .value = u_snprintf(params.field_values[RULE_ADVERTISING], sizeof(params.field_values[RULE_ADVERTISING]),
        " "BULLET" All advertising, unless approved of, is prohibited. *Please ask before posting*!")
  };

  params.fields[RULE_WARNING] = (struct discord_embed_field)
  {
    .name = u_snprintf(params.field_names[RULE_WARNING], sizeof(params.field_names[RULE_WARNING]), ""ACORNS" Warning Policy"),
    .value = u_snprintf(params.field_values[RULE_WARNING], sizeof(params.field_values[RULE_WARNING]),
        " Moderators (<@&1017203889784750100>) reserve the right to remove a member if they deem it necessary. \n"
        " "BULLET" We generally follow **Warn** (1st) -> **Kick** (2nd) -> **Ban** (3rd) depending on severity.")
  };

  struct sd_header_params header = { 0 };

  header.embed = (struct discord_embed) 
  {
    .color = (int)strtol("0EDF0A", NULL, 16),
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), user->username),
      .icon_url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          user->id, user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Rules"),
    .description = u_snprintf(params.description, sizeof(params.description), "Before we get started, let's establish some rules first."),
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(params.image_url, sizeof(params.image_url), GIT_PATH, 
          misc[MISC_WELCOME].file_path)
    },
    .fields = &(struct discord_embed_fields) {
      .array = params.fields,
      .size = RULE_SIZE
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_txt, sizeof(params.footer_txt), "These rules are subject to change!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, misc[MISC_ALERT].file_path)
    }
  };

  struct discord_create_message rules_msg = (struct discord_create_message) {
    .embeds = &(struct discord_embeds) {
      .array = &header.embed,
      .size = 1
    }
  };

  discord_create_message(client, RULES_CHANNEL_ID, &rules_msg, NULL);
}

void rules_interaction(struct discord *client, const struct discord_message *event) 
{
  // if (event->author->bot || event->channel_id != RULES_CHANNEL_ID)
  //   return;

  struct discord_ret_user ret_bot = { 
    .done = &create_rules,
    .fail = &not_user,
    .keep = event
  };

  // always use the production bot's ID
  discord_get_user(client, MAIN_BOT_ID, &ret_bot);
}