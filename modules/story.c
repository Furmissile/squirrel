int biome_story_interaction(const struct discord_interaction *event)
{
  struct sd_player player = { 0 };
  load_player_struct(&player, event->member->user->id);

  struct sd_help_info params = { 0 };

  int page_num = (event->data->custom_id) ? (event->data->custom_id[1] -48) +1 : 1;

  switch (page_num -1)
  {
    case STORY_GRASSLANDS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""GRASSLANDS_ICON" Grasslands"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" The Grasslands is a lushious green forest with flourishing wildlife and fertile soils. The land is so rich, sometimes the trees drop golden acorns!"
            " It is found in the east continent between the savanna to the south and mountains to the north. \n"

            " "INDENT" Imperium City is the main place of inhabitance for people. It's a beautiful city made entirely of marble. Known for their history of knowledge,"
            " they've always had a keen interest for the nearby Enchanted Woods with thriving mythical creatures. \n"

            " "INDENT" Although a beautiful city, it has its fair share of problems. Common instances include banits raiding farms, pollution, a high rodent population,"
            " and ruthless witches among others. These witches are known for kidnapping citizens for their experiments and playing around with dangerous corrupt magic.")
      };
      break;
    case STORY_SEEPING_SANDS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""SEEPING_SANDS_ICON" Seeping Sands"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" The Seeping Sands is a vast desert wasteland with little to no vegetation. It once had fertile soil, but eroded due to the malpractices of local farming."
            " The desert takes up the entire south continent and primarily consists of sand dunes where giant worms roam. \n"

            " "INDENT" The Forgotten City is the main place of inhabitance for people. It's a city made of sandstone and is called the Forgotten City because"
            " it's almost completely covered in sand. \n"

            " "INDENT" The locals claim the city was once prosperous, but they've struggled to get by due to the eroded soil and frequent unnatural-looking worm attacks. As a result, they"
            " have become reliant on imports for their supplies and provisions. They also claim to have seen living skeletons at night...")
      };
      break;
    case STORY_NATURE_END:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""NATURE_END_ICON" Nature's End"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" Nature's End is a chaotic biome characterized by gloomy skies, heavy and unpredictable rain storms, and earthquakes. It's part of the north"
            " continent taking up the southwest quadrant. It gets its name from the neighboring corruption consuming the wildlife. \n"

            " "INDENT" The local oracle believes the biome wasn't always like this; that it was once like the Grasslands and took up the south half of the continent."
            " And it only changed when the corruption began taking over. While the oracle's word has great influence on the locals in the small town, they don't all believe him. \n"

            " "INDENT" The City of Luxos was once the main inhabitance for people, but it fell to the unfortunate rise of the corruption. Even today, the corruption continues"
            " to overgrow this area. Now, the oracle does whatever he can to preserve what wildlife is left and prevent the whole continent from being devoured. No pressure!")
      };
      break;
    case STORY_DEATH_GRIP:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""DEATH_GRIP_ICON" Death's Grip"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" Death's Grip is a desolate tundra with little life. It takes the upper half of the north continent. It gets its name from its naturally unforgiving climate, and"
            " recently, the growing presence of the corruption. What was once a white and bright tundra is now riddled with necrotic energy and heavy fog. \n"

            " "INDENT" The Kingdom of Ice is the main place of inhabitance for people. The kingdom as the name suggests is entirely built of snow and ice. But since the rise of the"
            " corruption, it has taken on a purple hue. The citizens eventually caught word that the corruption was a growing threat and fled. Now mostly empty, the people left"
            " are constantly fending off the necrotic creatures to keep their home safe. There also exists the forgotten Lost City of Glory that appeared similar to the Kingdom of Ice,"
            " but fell much sooner to unresolved internal problems and the pressure of the corruption.")
      };
      break;
    case STORY_LAST_ACORN:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""LAST_ACORN_ICON" The Last Acorn"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" The Last Acorn a biome that looks quite like the Grasslands, but is characterized by dense purple fog, humming dark energy, blood of the sacrificed coursing"
            " through the dirt, and crazed squirrels who once inhabited the area. Now large enough to be a biome of its own, the corruption continues to take over its neighboring biomes."
            " It is found on the north continent in the southeast quadrant. \n"

            " "INDENT" Its name came from the squirrel colonies because squirrels are lured in by conjured acorns and it's the last acorn they ever see. It's rumored to have started because"
            " of a group of cultists who opened a portal to Hell, but this is up for debate. Others believe they unknowingly or accidently created an uncontrollable dark matter. No human"
            " has even been able to document it because they never came out alive.")
      };
      break;
    case STORY_ORIGINS:
      params.field = (struct discord_embed_field) {
        .name = u_snprintf(params.field_name, sizeof(params.field_name), 
            ""ACORNS" Origins of Squirrel Dash"),
        .value = u_snprintf(params.field_value, sizeof(params.field_value),
            " "INDENT" Before the question starts popping up everywhere, as a player might be wondering where did the game idea come from? \n"
            " "INDENT" The idea started off as a fishing game where you explore the same biomes while fishing as a cat! The problem I had here was that there wasn't any"
            " rationality behind how you progress through biomes by fishing; it just sounds like pond hopping. But I wanted more out of it. \n"

            " "INDENT" There has been an ongoing joke with some online friends and even within my family that one of my cats, Bunny, is running a catnip business and he uses the local squirrels to carry out his dirty work."
            " So I decided to jokingly make a game out of it. And for a time, I did! You explored the biomes collecting different themed squirrels and create your own network as a cat!"
            " But there was still no way to connect the story. So I passed the perspective to the eyes of the squirrel and sure enough, Squirrel Dash was born!")
      };
      break;
  }

  init_help_buttons(event, &params, page_num -1, TYPE_BIOME_STORY, STORY_TOPIC_SIZE -1);

  struct sd_header_params header = { 0 };

  char image_url[128] = { };

  header.embed = (struct discord_embed) 
  {
    .color = player.color,
    .author = &(struct discord_embed_author) {
      .name = u_snprintf(header.username, sizeof(header.username), event->member->user->username),
      .url = u_snprintf(header.avatar_url, sizeof(header.avatar_url), 
          "https://cdn.discordapp.com/avatars/%lu/%s.png",
          event->member->user->id, event->member->user->avatar)
    },
    .title = u_snprintf(header.title, sizeof(header.title), "Biomes (%d of %d)", 
        page_num, STORY_TOPIC_SIZE),
    .image = &(struct discord_embed_image) {
      .url = u_snprintf(image_url, sizeof(image_url), GIT_PATH, biomes[page_num -1].biome_scene_path)
    },

    .fields = &(struct discord_embed_fields) {
      .array = &params.field,
      .size = 1
    },
    .footer = &(struct discord_embed_footer) {
      .text = u_snprintf(params.footer_text, sizeof(params.footer_text), "Happy Foraging!"),
      .icon_url = u_snprintf(params.footer_url, sizeof(params.footer_url), GIT_PATH, items[ITEM_ACORNS].file_path)
    }
  };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = params.buttons,
      .size = 4
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &header.embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  char values[16384];
  CCORDcode code = discord_interaction_response_to_json(values, sizeof(values), &interaction);
  fprintf(stderr, "%s \nCCODE: %d \n", values, code);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  return 0;
}