PGresult* SQL_query(PGresult* query, char* format, ...)
{
  char buffer[2048] = { };

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, sizeof(buffer), format, args);

  va_end(args);

  query = PQexec(conn, buffer);

  if (PQresultStatus(query) != PGRES_COMMAND_OK)
  {
    FILE *fp = fopen("crash.log", "a");
    fprintf(fp, "%s \n", PQerrorMessage(conn));
    fclose(fp);
  }

  return query;
}

char* u_snprintf(char* buffer, size_t size, char* format, ...)
{
  va_list args;

  va_start(args, format);

  vsnprintf(buffer + strlen(buffer), size - strlen(buffer), format, args);

  va_end(args);

  return buffer;
}

unsigned long strtobigint(const char* str)
{
  if (!str)
    return -1;

  for (size_t i = 0; i < strlen(str); i++)
    if (str[i] -48 < 0
      || str[i] -48 > 9)
      return -1;

  return strtol(str, NULL, 10);
}

void trim_user_id(char dest[], size_t dest_size, char* input)
{
  char tmp_buffer[64] = { };

  if (dest_size < sizeof(tmp_buffer)) {
    fprintf(stderr, "TRIM_USER_ID:: BUFFER ERROR \n");
    return;
  }

  if (!strstr(input, "<@") && input[strlen(input) -1] != '>')
    snprintf(tmp_buffer, sizeof(tmp_buffer), "<@%s>", input);
  else if (!strstr(input, "<@") || input[strlen(input) -1] != '>')
  {
    fprintf(stderr, "TRIM_USER_ID:: STR ERROR \n");
    return;
  }
  else 
    snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", input);

  int user_id_i = 2;
  while (tmp_buffer[user_id_i] != '>' && tmp_buffer[user_id_i])
  {
    dest[user_id_i -2] = tmp_buffer[user_id_i];
    user_id_i++;
  }
}

void trim_buffer(char dest[], size_t dest_size, char* input, char separator)
{
  char tmp_buffer[64] = { };

  if (dest_size < sizeof(tmp_buffer)) {
    fprintf(stderr, "TRIM_BUFFER:: BUFFER ERROR \n");
    return;
  }
  snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", input);
  int tmp_buffer_size = strlen(tmp_buffer);

  int tmp_buffer_idx = 0;
  while (tmp_buffer[tmp_buffer_idx] != separator
    && tmp_buffer_idx < tmp_buffer_size)
    tmp_buffer_idx++;

  if (tmp_buffer_idx == tmp_buffer_size) {
    fprintf(stderr, "TRIM_BUFFER:: NO SEPARATOR \n");
    return;
  }

  tmp_buffer_idx++;

  int buffer_idx = 0;
  while (tmp_buffer[tmp_buffer_idx] != separator)
  {
    if (tmp_buffer_idx == tmp_buffer_size)
      break;
    dest[buffer_idx++] = tmp_buffer[tmp_buffer_idx++];
  }
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

void num_str(char dest[], size_t dest_size, unsigned long input) 
{
  char tmp_buffer[64];

  if (dest_size < sizeof(tmp_buffer)) {
    fprintf(stderr, "NUM_STR:: BUFFER ERROR \n");
    return;
  }

  snprintf(tmp_buffer, sizeof(tmp_buffer), "%ld", input);

  size_t tmp_buf_size = strlen(tmp_buffer);

  // if num is less than 1000, return number
  if (tmp_buf_size < 4) {
    snprintf(dest, dest_size, "%s", tmp_buffer);
    return;
  }

  //define number offset by remainder of 3
  size_t first_set_n = (tmp_buf_size % 3 == 0) ? 3 : tmp_buf_size % 3;

  size_t buffer_offset = strlen(dest);
  //apply offset and add comma
  for (size_t tmp_offset = 0; tmp_offset < first_set_n; tmp_offset++)
    dest[buffer_offset++] = tmp_buffer[tmp_offset];
  dest[buffer_offset++] = ',';

  //while there are still characters, add 3 numbers and a comma
  for (size_t tmp_offset = 0; tmp_offset < tmp_buf_size -first_set_n; tmp_offset++) {
    dest[buffer_offset++] = tmp_buffer[tmp_offset + first_set_n];

    //do not apply comma if at end of string
    if (((tmp_offset +1) % 3 == 0) && (tmp_offset + first_set_n != tmp_buf_size -1))
      dest[buffer_offset++] = ',';
  }

}

void lowercase(char dest[], size_t dest_size, char* input)
{
  if (dest_size < strlen(input)) {
    fprintf(stderr, "LOWERCASE:: BUFFER ERROR \n");
    return;
  }
  for (size_t i = 0; i < strlen(input); i++)
    dest[i] = (input[i] < 91 && input[i] > 64) ? input[i] + 32 : input[i];
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


void not_user(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  error_message(event, "This is not a valid user!");
}

struct tm* get_UTC()
{
  time_t rawtime;
  struct tm *info;
  time(&rawtime);
  info = gmtime(&rawtime);

  return info;
}