char* format_str(int size, char* format, ...)
{
  char* buffer = calloc(size, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, size, format, args);

  va_end(args);

  return buffer;
}

PGresult* SQL_query(PGconn* db_conn, char* format, ...)
{
  char* buffer = calloc(SIZEOF_SQL_COMMAND, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, SIZEOF_SQL_COMMAND, format, args);

  va_end(args);

  PGresult* query = PQexec(db_conn, buffer);

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

/* Returns a total price based on stat level, currency unit, and aa value multiplier */
int generate_price(int stat_lv, double value_mult)
{
  return (UNIT_ACORN * stat_lv * value_mult) + (stat_lv/STAT_EVOLUTION * UNIT_ACORN);
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

  for (size_t i = 1; i < max_len +1; i++)
  {
    int base_multiplier = 1;
    for (size_t x = 1; x < i; x++)
      base_multiplier *= 10;

    num += (str[max_len - i] - 48) * base_multiplier;
  }

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

  if (energy_loss == STEAL_ENERGY_COST
    || (rand() % MAX_CHANCE < 80 && energy_loss == MAIN_ENERGY_COST) )
  {
    player.energy -= energy_loss;

    embed->footer = calloc(1, sizeof(struct discord_embed_footer));
    embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "You have %d energy left!", player.energy),
    embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ENERGY].item.file_path);
    
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n-**%d** "ENERGY" Energy \n", energy_loss);
  }
  else if (energy_loss == MAIN_ENERGY_COST) 
  {
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n"ENERGY" No energy was lost! \n");
  }
  else {
    printf("An invalid energy value was passed! \n");
  }
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

  int add_energy = (player.energy < MAX_ENERGY) ? (time(NULL) - player.main_cd)/ BASE_ENERGY_CD : 0;

  if (player.energy + add_energy > MAX_ENERGY)
    player.energy += (player.energy > MAX_ENERGY) ? 0 : add_energy - (add_energy - (MAX_ENERGY - player.energy));
  else
    player.energy += add_energy;

  // wont disturb cooldown when info embed is sent
  player.main_cd = time(NULL) - ((time(NULL) - player.main_cd) % BASE_ENERGY_CD);
}

void not_user(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  error_message(event, "This is not a valid user!");
}