void print_pie(struct sd_pie *pie)
{
  printf("\n%d %d %d \n%d   %d \n%d %d %d \n\n",
      pie->decoded_buf[SLICE_TOP_LEFT], 
      pie->decoded_buf[SLICE_TOP_CENTER], 
      pie->decoded_buf[SLICE_TOP_RIGHT],
      pie->decoded_buf[SLICE_MID_LEFT],
      pie->decoded_buf[SLICE_MID_RIGHT],
      pie->decoded_buf[SLICE_BOTTOM_LEFT],
      pie->decoded_buf[SLICE_BOTTOM_CENTER],
      pie->decoded_buf[SLICE_BOTTOM_RIGHT] );
}

char* format_pie(struct sd_pie *pie)
{
  u_snprintf(pie->formatted_pie, sizeof(pie->formatted_pie), "\n%s%s%s \n%s"INDENT"%s \n%s%s%s \n",
      slice_types[pie->decoded_buf[SLICE_TOP_LEFT]].emoji_literal, 
      slice_types[pie->decoded_buf[SLICE_TOP_CENTER]].emoji_literal, 
      slice_types[pie->decoded_buf[SLICE_TOP_RIGHT]].emoji_literal,
      slice_types[pie->decoded_buf[SLICE_MID_LEFT]].emoji_literal,
      slice_types[pie->decoded_buf[SLICE_MID_RIGHT]].emoji_literal,
      slice_types[pie->decoded_buf[SLICE_BOTTOM_LEFT]].emoji_literal,
      slice_types[pie->decoded_buf[SLICE_BOTTOM_CENTER]].emoji_literal,
      slice_types[pie->decoded_buf[SLICE_BOTTOM_RIGHT]].emoji_literal );

  return pie->formatted_pie;
}

void encode_pie(struct sd_pie *pie)
{
  snprintf(pie->encoded_buf, sizeof(pie->encoded_buf), PIE_FORMAT,
      pie->decoded_buf[SLICE_TOP_LEFT], 
      pie->decoded_buf[SLICE_TOP_CENTER], 
      pie->decoded_buf[SLICE_TOP_RIGHT],
      pie->decoded_buf[SLICE_MID_RIGHT],
      pie->decoded_buf[SLICE_BOTTOM_RIGHT],
      pie->decoded_buf[SLICE_BOTTOM_CENTER],
      pie->decoded_buf[SLICE_BOTTOM_LEFT],
      pie->decoded_buf[SLICE_MID_LEFT]);
}

void decode_pie(struct sd_pie *pie)
{
  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    int slice_type = pie->encoded_buf[slice_idx] -48;

    if (slice_type > TYPE_EMPTY)
      pie->decoded_buf[slice_idx] = slice_type;
  }
}

/*
  rotates the pie clockwise ~90 degrees
*/
void rotate_pie(struct sd_pie *pie)
{
  // save old buffer before clearing!
  int old_decoded_buf[SLICES_SIZE] = { };
  for (int i = 0; i < SLICES_SIZE; i++)
  {
    old_decoded_buf[i] = pie->decoded_buf[i];
  }

  // always start with a fresh buffer!
  *pie = (struct sd_pie) { 0 };

  int slice_idx = 0;

  // bring end pieces to beginning
  for (; slice_idx < 2; slice_idx++)
  {
    pie->decoded_buf[slice_idx] = old_decoded_buf[SLICES_SIZE - 2 + slice_idx];
  }

  // move rest of pieces
  for (; slice_idx < SLICES_SIZE; slice_idx++)
  {
    pie->decoded_buf[slice_idx] = old_decoded_buf[slice_idx - 2];
  }

  encode_pie(pie);
}

/*
  checks if piece can fit into dest_piece and returns 0 if it can fit
*/
int check_piece_fit(struct sd_pie *dest_pie, struct sd_pie *piece)
{
  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    if (dest_pie->decoded_buf[slice_idx] > 0
      && piece->decoded_buf[slice_idx] > 0)
    {
      return 1;
    }
  }

  return 0;
}

/*
  merge piece into dest_piece only where piece is filled
*/
void merge_pies(struct sd_pie *dest_pie, struct sd_pie *piece)
{
  // merge pie indexes ONLY if piece is not empty!
  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    if (piece->decoded_buf[slice_idx] > 0)
      dest_pie->decoded_buf[slice_idx] = piece->decoded_buf[slice_idx];
  }

  encode_pie(dest_pie);
}

/*
  generates a new piece by merging piece into dest_pie
*/
void generate_new_piece(struct sd_pie *dest_pie, struct sd_pie *piece)
{
  *dest_pie = (struct sd_pie) { 0 };

  merge_pies(dest_pie, piece);

  struct tm *info = get_UTC();
  int seasonal_reward = (info->tm_mday < 7) ? TYPE_SPRING_SEEDS :
    (info->tm_mday < 14) ? TYPE_SUMMER_CHERRIES :
    (info->tm_mday < 21) ? TYPE_FALL_BLUEBERRIES : TYPE_WINTER_CATNIP;

  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    // chance for certain item types!
    if (dest_pie->decoded_buf[slice_idx] > 0)
    {
      int chance = rand() % MAX_CHANCE;

      dest_pie->decoded_buf[slice_idx] = (chance < COMMON_CHANCE) ? TYPE_ACORNS :
        (chance < EVENT_CHANCE) ? seasonal_reward :
        (chance < RARE_CHANCE) ? TYPE_GOLDEN_ACORNS : TYPE_CONJURED_ACORNS;

      // chance separately for ribboned acorn
      if (rand() % MAX_CHANCE > EVENT_CHANCE && info->tm_mon == CHRISTMAS_MONTH)
        dest_pie->decoded_buf[slice_idx] = TYPE_RIBBONED_ACORN;
    }
  }

  encode_pie(dest_pie);
}

/*
  scans history returning 0 if slice_idx doesnt exist
*/
int search_history(char history[64], int slice_idx)
{
  if (strchr(history, slice_idx +48))
    return 1;

  return 0;
}

/*
  move next piece to current piece by clearing current and merging next

  makes sure there are not any repeated pieces by checking history
*/
void shift_pieces(struct sd_pie_game *pie_game)
{
  // clear current pie
  pie_game->current_piece = (struct sd_pie) { 0 };

  // merge next to current
  merge_pies(&pie_game->current_piece, &pie_game->next_piece);

  // pick a new piece that hasnt been used yet  
  int next_piece = rand() % PIECES_SIZE;
  while (search_history(pie_game->history, next_piece) == 1)
    next_piece = rand() % PIECES_SIZE;

  // determine types of each slice
  generate_new_piece(&pie_game->next_piece, &pieces[next_piece]);

  pie_game->piece_count++;

  // add next piece to history (-1 for encounter idx)
  int history_size = strlen(pie_game->history) -1;

  pie_game->history[pie_game->piece_count % history_size] = (char)(next_piece +48);
}

int check_types(struct sd_pie *pie, int type)
{
  int count = 0;
  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    int acorn_type = pie->decoded_buf[slice_idx];

    if (acorn_type == type)
      count++;
  }
  return count;
}

int check_complete_pie(struct sd_pie *pie)
{
  int score = 0;

  for (int slice_idx = 0; slice_idx < SLICES_SIZE; slice_idx++)
  {
    int acorn_type = pie->decoded_buf[slice_idx];
    
    if (acorn_type == TYPE_EMPTY)
      return 0;
    else
      score += slice_types[acorn_type].base_value;
  }

  return score;
}

int check_end(struct sd_pie_game *game)
{
  struct sd_pie *pies[3] = { &game->pie_1, &game->pie_2, &game->pie_3 };

  // for each pie
  for (int pie_idx = 0; pie_idx < 3; pie_idx++)
  {
    struct sd_pie temp_pie = game->current_piece;

    // for each rotation
    for (int i = 0; i < 4; i++)
    {
      // return if piece doesnt fit
      if (check_piece_fit(pies[pie_idx], &temp_pie) == 0)
        return 0;

      rotate_pie(&temp_pie);
    }

  }

  return 1;
}
