/*

  This file handles the event help embed.

    - Displays info regarding events

*/

void e_help(const struct discord_interaction *event, struct sd_message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  embed->title = format_str(SIZEOF_TITLE, "Event Help (%d of %d)", page_num, E_TOPIC_SIZE);

  embed->color = player.color;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help on how events work? Check out the info below \n"
      "to see if they answer your questions!");

  struct discord_embed_field *event_help_pages = (struct discord_embed_field[])
  {
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Acorn Count Event"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" This is an ongoing event where players compete based on the amount of acorns they've collected! \n"
          " "OFF_ARROW" The count is uneffected by your stats and buffs, so be sure to have your luck in your right pocket! \n"
          " "OFF_ARROW" The count is increased by `/forage` and successful steals! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Seasons"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Seasons change every week and passively occur. Each season gives a different boost to acorn gains. \n"
          " "OFF_ARROW" Seasons include Spring (+**20**%%), Summer (+**10**%%), Fall (+**5**%%), and Winter (No boost). \n"
          " "OFF_ARROW" Send `/season_info` to find out when the current season ends! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Spring Chicken Event (Spring)"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Trees everywhere are dropping "GOLDEN_ACORNS" Golden Acorns! \n"
          " "OFF_ARROW" Lasts throughout the in-game Spring season! \n"
          " "OFF_ARROW" Golden acorns have a chance to be included in every `/forage` instead of just from `/steal` or lost stashes! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Garden Raid Event (Summer)"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Everyone is growing all sorts of useful victuals! *RaId ThEiR gArDeNsSs!* \n"
          " "OFF_ARROW" Lasts throughout the in-game Summer season! \n"
          " "OFF_ARROW" Look for all sorts of victuals including blueberries, seeds, and cherries for bonus rewards! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Winter Prep Event (Fall)"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Winter is coming! Take this time to snatch all the acorns you can before hibernation! \n"
          " "OFF_ARROW" Lasts throughout the in-game Fall season! \n"
          " "OFF_ARROW" Energy regeneration for all players is halved! \n")
    },
    {
      .name = format_str(SIZEOF_TITLE, ""ACORNS" Bunny's Endeavor (Winter)"),
      .value = format_str(SIZEOF_FIELD_VALUE,
          " "OFF_ARROW" Resources are depleting! Bunny is looking for able-bodies who can fetch him "CATNIP" catnip. He'll pay handsomely for it, too. \n"
          " "OFF_ARROW" Lasts throughout the in-game Winter season! Check out `/bunny_endeavor`! \n"
          " "OFF_ARROW" Catnip has a chance to be included in every `/forage`! \n")
    }
  };

  discord_msg->buttons = build_help_buttons(event, page_num -1, TYPE_EVENT_HELP, E_TOPIC_SIZE -1);

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = 1;
  embed->fields->array = calloc(1, sizeof(struct discord_embed_field));
  embed->fields->array[0] = event_help_pages[page_num -1];
  
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Happy Foraging!");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORNS].item.file_path);
}


/* Listens for slash command interactions */
int event_help_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  e_help(event, discord_msg);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = discord_msg->buttons
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(discord_msg->embed);
  free(discord_msg->buttons);
  free(discord_msg);

  return 0;
}