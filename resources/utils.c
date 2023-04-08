char* format_str(int size, char* format, ...)
{
  char* buffer = calloc(size, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, size, format, args);

  va_end(args);

  return buffer;
}

PGresult* SQL_query(enum ACTION_TYPE action, char* format, ...)
{
  char* buffer = calloc(SIZEOF_SQL_COMMAND, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, SIZEOF_SQL_COMMAND, format, args);

  va_end(args);

  PGresult* query = PQexec(conn, buffer);

  // also make the call to update the backup DB ONLY if it's not a search (so essentially an update)
  // (this way, we arent searching two matching databases)
  if (action != DB_ACTION_SEARCH)
    PQexec(backup_conn, buffer);

  free(buffer);

  return query;
}

struct discord_embed_author* sd_msg_embed_author(char* name, char* icon_url)
{
  struct discord_embed_author *author = calloc(1, sizeof(struct discord_embed_author));

  author->name = calloc(SIZEOF_TITLE, sizeof(char));
  author->icon_url = calloc(SIZEOF_URL, sizeof(char));

  author->name = name;
  author->icon_url = icon_url;

  return author;
}

/* Returns total factor based on stat level and if it's a multiplier or incrementer */
float generate_factor(int stat_lv, double base_value)
{
  if (base_value == STRENGTH_VALUE)
    return base_value * (stat_lv -1);
  else
    return (base_value * (stat_lv -1)) + (base_value * (stat_lv/STAT_EVOLUTION)) +1;
}

/* Returns a total price based on stat level, currency unit, and a value multiplier */
int generate_price(int stat_lv, double value_mult)
{
  // return (value_mult * stat_lv) + (value_mult * (stat_lv/STAT_EVOLUTION));
  return ((stat_lv +1)/STAT_EVOLUTION * value_mult + value_mult) * stat_lv;
}

/* Checks for matching ids to make sure the player that sent the embed is the one pressing a button */
void error_message(const struct discord_interaction *event, char* message) {
  struct discord_interaction_response error_message = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = message
    }
  };

  discord_create_interaction_response(client, event->id, event->token, &error_message, NULL);
}

unsigned long strtobigint(const char* str)
{
  if (!str)
    return -1;

  for (size_t i = 0; i < strlen(str); i++)
    if (str[i] -48 < 0
      || str[i] -48 > 9)
      return ERROR_STATUS;

  return strtol(str, NULL, 10);
}

char* trim_user_id(char* input)
{
  char* user_id = calloc(32, sizeof(char));

  char tmp_buffer[32] = {};
  if (!strstr(input, "<@") && input[strlen(input) -1] != '>')
    snprintf(tmp_buffer, sizeof(tmp_buffer), "<@%s>", input);
  else if (!strstr(input, "<@") || input[strlen(input) -1] != '>')
    return NULL;
  else 
    snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", input);

  int user_id_i = 2;
  while (tmp_buffer[user_id_i] != '>' && tmp_buffer[user_id_i])
  {
    user_id[user_id_i -2] = tmp_buffer[user_id_i];
    user_id_i++;
  }

  return user_id;
}

int strtoint(const char* str)
{
  int num = 0;
  size_t max_len = strlen(str);

  size_t is_negative = (str[0] == '-') ? 1 : 0;

  for (size_t str_idx = is_negative; str_idx < max_len; str_idx++)
  {
    size_t pos = max_len - str_idx - 1;
    int base_multiplier = 1;
    for (size_t x = 0; x < pos; x++)
      base_multiplier *= 10;

    num += ((str[str_idx] - 48) * base_multiplier);
  }

  if (is_negative)
    num *= -1;

  return num;
}

char* num_str(long long num) 
{
  char tmp_buffer[32];
  snprintf(tmp_buffer, sizeof(tmp_buffer), "%lld", num);

  char* buffer = calloc(32, sizeof(char));

  // if num is less than 1000, return number
  if (strlen(tmp_buffer) < 4) {
    snprintf(buffer, 32, "%s", tmp_buffer);
    return buffer;
  }
  //define number offset by remainder of 3
  size_t first_set_n = (strlen(tmp_buffer) % 3 == 0) ? 3 : strlen(tmp_buffer) % 3;

  size_t buffer_offset = strlen(buffer);

  //apply offset and add comma
  for (size_t tmp_offset = 0; tmp_offset < first_set_n; tmp_offset++)
    buffer[buffer_offset++] = tmp_buffer[tmp_offset];
  buffer[buffer_offset++] = ',';

  //while there are still characters, add 3 numbers and a comma
  for (size_t tmp_offset = 0; tmp_offset < strlen(tmp_buffer) -first_set_n; tmp_offset++) {
    buffer[buffer_offset++] = tmp_buffer[tmp_offset + first_set_n];

    //do not apply comma if at end of string
    if (((tmp_offset +1) % 3 == 0) && (tmp_offset + first_set_n != strlen(tmp_buffer) -1))
      buffer[buffer_offset++] = ',';
  }

  return buffer;
}

char* lowercase(const char* str)
{
  char* buffer = calloc(strlen(str) +1, sizeof(char));

  for (size_t i = 0; i < strlen(str); i++)
    buffer[i] = (str[i] < 91 && str[i] > 64) ? str[i] + 32 : str[i];

  return buffer;
}

void energy_status(struct sd_message *discord_msg, int energy_loss)
{
  struct discord_embed *embed = discord_msg->embed;
  int final_energy_loss = energy_loss;
  
  if (final_energy_loss == MAIN_ENERGY_COST)
  {
    if (player.squirrel == GRAY_SQUIRREL
      && (rand() % MAX_CHANCE) > 70)
    {
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, "\n"ENERGY" No energy was lost! \n");
      return;
    }
    else if (player.squirrel == KING_SQUIRREL)
    {
      final_energy_loss *= 2;
    }
  }

  player.energy -= final_energy_loss;

  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You have %d energy left!", player.energy),
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ENERGY].item.file_path);
  
  ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
      "\n-**%d** "ENERGY" Energy \n", final_energy_loss);
}

struct tm* get_UTC()
{
  time_t rawtime;
  struct tm *info;
  time(&rawtime);
  info = gmtime(&rawtime);

  return info;
}

void energy_regen() 
{
  if (player.buffs.endurance_acorn > 0)
  {
    player.energy += player.buffs.endurance_acorn;
    player.buffs.endurance_acorn = 0;
  }
  if (player.buffs.strength_acorn > 0)
  {
    player.health += player.buffs.strength_acorn;
    player.buffs.strength_acorn = 0;
  }

  struct tm *info = get_UTC();

  int energy_cd = (info->tm_mday > 7 && info->tm_mday < 14) ? 90 : 180;

  int add_energy = (player.energy < MAX_ENERGY) ? (time(NULL) - player.main_cd)/ energy_cd : 0;

  if (player.energy + add_energy > MAX_ENERGY)
    player.energy += (player.energy > MAX_ENERGY) ? 0 : add_energy - (add_energy - (MAX_ENERGY - player.energy));
  else
    player.energy += add_energy;

  // wont disturb cooldown when info embed is sent
  player.main_cd = time(NULL) - ((time(NULL) - player.main_cd) % energy_cd);
}

void not_user(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  error_message(event, "This is not a valid user!");
}


/*
  The following functions are only utilized in the support server!
*/

// mechanic for reaction verification
void verify(struct discord *client, const struct discord_message_reaction_add *event)
{
  if (event->member->user->id == APPLICATION_ID || event->channel_id != VERIFY_CHANNEL_ID)
    return;

  // delete whatever emoji user reacted with
  discord_delete_user_reaction(client, event->channel_id, event->message_id, event->member->user->id, 
      event->emoji->id, event->emoji->name, NULL);

  // only assign member role if they reacted with the acorn
  if (event->emoji->id != items[ITEM_ACORNS].item.emoji_id)
    return;

  discord_add_guild_member_role(client, GUILD_ID, event->member->user->id, MEMBER_ROLE_ID, 
  &(struct discord_add_guild_member_role) { .reason = "New member" }, 
  NULL);
}

void welcome_embed(struct discord *client, const struct discord_guild_member *event)
{
  if (event->guild_id != GUILD_ID) return;

  struct discord_embed *embed = calloc(1, sizeof(struct discord_embed));

  //Load Author
  embed->author = sd_msg_embed_author(
      format_str(SIZEOF_TITLE, event->user->username),
      format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
          event->user->id, event->user->avatar) );

  // color must be flat since this is a module
  embed->color = (int)strtol("00eeff", NULL, 16);

  embed->title = format_str(SIZEOF_TITLE, "Welcome to Squirrel Dash!");

  embed->thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail));
  embed->thumbnail->url = format_str(SIZEOF_URL, GIT_PATH, squirrels[GRAY_SQUIRREL].squirrel.file_path);

  embed->description = format_str(SIZEOF_DESCRIPTION,
      " "OFF_ARROW" Read the <#1046640388456321126> to <#1046813534790635550>! \n"
      " "OFF_ARROW" Begin your adventure in <#1046635264883294259>! \n"
      " "OFF_ARROW" Chat with fellow squirrel advocators in <#1046628380222685255> \n"
      " "OFF_ARROW" Looking for extra help? Ask away in <#1047233819201261748>!");

  embed->image = calloc(1, sizeof(struct discord_embed_image));
  embed->image->url = format_str(SIZEOF_URL, GIT_PATH, WELCOME_MSG_PATH);

  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Happy Foraging!");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORNS].item.file_path);

  struct discord_create_message intro_message = (struct discord_create_message) {
    .content = format_str(SIZEOF_DESCRIPTION, "*<@%ld> is looking for acorns...*", event->user->id),
    .embeds = &(struct discord_embeds) {
      .array = embed,
      .size = 1
    }
  };

  discord_create_message(client, WELCOME_CHANNEL_ID, &intro_message, NULL);
  free(embed);
}