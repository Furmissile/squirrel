/*

  This file handles the event help embed.

    - Displays info regarding events

*/

// other event are to be added (including holiday events, etc.)
enum E_TOPIC {
  E_TOPIC_SEASONS,
  E_TOPIC_ACORN_COUNT,
  E_TOPIC_SPRING,
  E_TOPIC_SUMMER,
  E_TOPIC_FALL,
  E_TOPIC_WINTER,
  E_TOPIC_SIZE
};

void e_help(struct sd_message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Event Help");

  embed->color = player.color;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help on how events work? Check out the info below \n"
      "to see if they answer your questions!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = E_TOPIC_SIZE;
  embed->fields->array = calloc(E_TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[E_TOPIC_ACORN_COUNT].name = format_str(SIZEOF_TITLE, ""ACORNS" Acorn Count Event");
  embed->fields->array[E_TOPIC_ACORN_COUNT].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" This is an ongoing event where players compete based on the amount of acorns they've collected! \n"
      " "OFF_ARROW" The count is uneffected by your stats and buffs, so be sure to have your luck in your right pocket! \n"
      " "OFF_ARROW" The count is increased by `/forage` and successful steals! \n");

  embed->fields->array[E_TOPIC_SEASONS].name = format_str(SIZEOF_TITLE, ""ACORNS" Seasons");
  embed->fields->array[E_TOPIC_SEASONS].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Seasons change every week and passively occur. Each season gives a different boost to acorn gains. \n"
      " "OFF_ARROW" Seasons include Spring (+**20**%%), Summer (+**10**%%), Fall (+**5**%%), and Winter (No boost). \n"
      " "OFF_ARROW" Send `/season_info` to find out when the current season ends! \n");
  
  embed->fields->array[E_TOPIC_SPRING].name = format_str(SIZEOF_TITLE, ""ACORNS" Spring Chicken Event (Spring)");
  embed->fields->array[E_TOPIC_SPRING].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Trees everywhere are dropping "GOLDEN_ACORNS" Golden Acorns! \n"
      " "OFF_ARROW" Lasts throughout the in-game Spring season! \n"
      " "OFF_ARROW" Golden acorns have a chance to be included in every `/forage` instead of just from `/steal` or lost stashes! \n");

  embed->fields->array[E_TOPIC_SUMMER].name = format_str(SIZEOF_TITLE, ""ACORNS" Garden Raid Event (Summer)");
  embed->fields->array[E_TOPIC_SUMMER].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Everyone is growing all sorts of useful victuals! *RaId ThEiR gArDeNsSs!* \n"
      " "OFF_ARROW" Lasts throughout the in-game Summer season! \n"
      " "OFF_ARROW" Look for all sorts of victuals including blueberries, seeds, and cherries for bonus rewards! \n");

  embed->fields->array[E_TOPIC_FALL].name = format_str(SIZEOF_TITLE, ""ACORNS" Winter Prep Event (Fall)");
  embed->fields->array[E_TOPIC_FALL].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Winter is coming! Take this time to snatch all the acorns you can before hibernation! \n"
      " "OFF_ARROW" Lasts throughout the in-game Fall season! \n"
      " "OFF_ARROW" Energy regeneration for all players is halved! \n");

  embed->fields->array[E_TOPIC_WINTER].name = format_str(SIZEOF_TITLE, ""ACORNS" Bunny's Endeavor (Winter)");
  embed->fields->array[E_TOPIC_WINTER].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Resources are depleting! Bunny is looking for able-bodies who can fetch him "CATNIP" catnip. He'll pay handsomely for it, too. \n"
      " "OFF_ARROW" Lasts throughout the in-game Winter season! Check out `/bunny_endeavor`! \n"
      " "OFF_ARROW" Catnip has a chance to be included in every `/forage`! \n");
  
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

  e_help(discord_msg);

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = discord_msg->embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  free(discord_msg);

  return 0;
}