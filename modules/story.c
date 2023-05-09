/*

  This file handles displaying the story for each biome

*/

void biome_story(const struct discord_interaction *event, struct sd_message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  embed->title = format_str(SIZEOF_TITLE, "Story (%d of %d)", page_num, STORY_TOPIC_SIZE);

  embed->color = player.color;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Curious about the origins of Squirrel Dash? Check out these pages for some backstory!");

  char* biome_story_pages[STORY_TOPIC_SIZE] = {
    // STORY_GRASSLANDS
    " "INDENT" The Grasslands is a lushious green forest with flourishing wildlife and fertile soils. The land is so rich, sometimes the trees drop golden acorns!"
    " It is found in the east continent between the savanna to the south and mountains to the north. \n"

    " "INDENT" Imperium City is the main place of inhabitance for people. It's a beautiful city made entirely of marble. Known for their history of knowledge,"
    " they've always had a particular interest for the nearby Enchanted Woods with thriving mythical creatures. \n"

    " "INDENT" Although a beautiful city, it has its fair share of problems. Common instances include rebels raiding farms, pollution, a high rodent population,"
    " and ruthless witches. These witches are known for kidnapping citizens for their experiments and playing around with dangerous corrupt magic.",

    // STORY_SEEPING_SANDS    
    " "INDENT" The Seeping Sands is a vast desert wasteland with little to no vegetation. It once had fertile soil, but eroded due to the malpractices of local farming."
    " The desert takes up the entire south continent and primarily consists of sand dunes where giant worms roam. \n"

    " "INDENT" The Forgotten City is the main place of inhabitance for people. It's a city made of sandstone and is called the Forgotten City because"
    " it's almost completely covered in sand. \n"

    " "INDENT" The locals claim the city was once prosperous, but they've struggled to get by due to the eroded soil and frequent unnatural-looking worm attacks. As a result, they"
    " have become reliant on imports for their supplies and provisions. They also claim to have seen living skeletons at night...",

    // STORY_NATURE_END
    " "INDENT" Nature's End is a chaotic biome characterized by gloomy skies, heavy and unpredictable rain storms, and earthquakes. It's part of the north"
    " continent taking up the southwest quadrant. It gets its name from the neighboring corruption consuming the wildlife. \n"

    " "INDENT" The local oracle believes the biome wasn't always like this; that it was once like the Grasslands and took up the south half of the continent."
    " And it only changed when the corruption began taking over. While the oracle's word has great influence on the locals in the small town, they don't all believe him. \n"

    " "INDENT" The City of Luxos was once the main inhabitance for people, but it fell to the unfortunate rise of the corruption. Even today, the corruption continues"
    " to overgrow this area. Now, the oracle does whatever he can to preserve what wildlife is left and prevent the whole continent from being devoured. No pressure!",

    // STORY_DEATH_GRIP
    " "INDENT" Death's Grip is a desolate tundra with little life. It takes the upper half of the north continent. It gets its name from its naturally unforgiving climate, and"
    " recently, the growing presence of the corruption. What was once a white and bright tundra is now riddled with necrotic energy and heavy fog. \n"

    " "INDENT" The Kingdom of Ice is the main place of inhabitance for people. The kingdom as the name suggests is entirely built of snow and ice. But since the rise of the"
    " corruption, it has taken on a purple hue. The citizens eventually caught word that the corruption was a growing threat and fled. Now mostly empty, the people left"
    " are constantly fending off the necrotic creatures to keep their home safe. There also exists the forgotten Lost City of Glory that appeared similar to the Kingdom of Ice,"
    " but fell much sooner to unresolved internal problems and the pressure of the corruption.",

    // STORY_LAST_ACORN
    " "INDENT" The Last Acorn a biome that looks quite like the Grasslands, but is characterized by dense purple fog, humming dark energy, blood of the sacrificed coursing"
    " through the dirt, and crazed squirrels who once inhabited the area. Now large enough to be a biome of its own, the corruption continues to take over its neighboring biomes."
    " It is found on the north continent in the southeast quadrant. \n"

    " "INDENT" Its name came from the squirrel colonies because squirrels are lured in by conjured acorns and it's the last acorn they ever see. It's rumored to have started because"
    " of a group of cultists who opened a portal to Hell, but this is up for debate. Others believe they unknowingly or accidently created an uncontrollable dark matter. No human"
    " has even been able to document it because they never came out alive."
  };

  discord_msg->buttons = build_help_buttons(event, page_num -1, TYPE_BIOME_STORY, STORY_TOPIC_SIZE -1);

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = 1;
  embed->fields->array = calloc(1, sizeof(struct discord_embed_field));

  struct sd_file_data biome_icon = biomes[page_num -1].biome_icon;
  embed->fields->array[0].name = format_str(SIZEOF_TITLE, "<:%s:%ld> %s",
        biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name);
  embed->fields->array[0].value = format_str(SIZEOF_FIELD_VALUE, biome_story_pages[page_num - 1]);
  
  embed->footer = calloc(1, sizeof(struct discord_embed_footer));
  embed->footer->text = format_str(SIZEOF_FOOTER_TEXT, "Happy Foraging!");
  embed->footer->icon_url = format_str(SIZEOF_URL, GIT_PATH, items[ITEM_ACORNS].item.file_path);
}

/* Listens for slash command interactions */
int biome_story_interaction(
  const struct discord_interaction *event, 
  struct sd_message *discord_msg) 
{
  //Load Author
  discord_msg->embed->author = sd_msg_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  biome_story(event, discord_msg);

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