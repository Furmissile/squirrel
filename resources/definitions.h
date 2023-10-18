enum DB_TUPLE
{
  DB_USER_ID = 0,
  DB_SCURRY_ID,
  DB_COLOR,
  DB_SQUIRREL,
  DB_ENERGY,
  DB_HEALTH,
  DB_ACORNS,
  DB_ACORN_COUNT,
  DB_HIGH_ACORN_COUNT,
  DB_GOLDEN_ACORNS,
  DB_STOLEN_ACORNS,
  DB_ENCOUNTER,
  DB_MAIN_CD,
  DB_CONJURED_ACORNS,
  DB_CATNIP,
  DB_SECTION,
  DB_SPENT_GOLDEN_ACORNS,
  // skip user id
  DB_PROFICIENCY_LV = 18,
  DB_STRENGTH_LV,
  DB_LUCK_LV,
  // skip user id
  DB_PROFICIENCY_ACORN = 22,
  DB_LUCK_ACORN,
  DB_BOOSTED_ACORN,
  // skip user id
  DB_GL_STORY = 26,
  DB_SP_STORY,
  DB_NE_STORY,
  DB_DG_STORY,
  DB_LA_STORY,

  DB_SESSION_START_TIME = 32,
  DB_SESSION_ACORN_COUNT,
  DB_SESSION_ACORNS,
  DB_SESSION_GOLDEN_ACORNS,
  DB_SESSION_HEALTH_LOSS,
  DB_SESSION_NO_ACORNS,
  DB_SESSION_ACORN_HANDFUL,
  DB_SESSION_ACORN_MOUTHFUL,
  DB_SESSION_LOST_STASH,
  DB_SESSION_ACORN_SACK
};

enum SCURRY_RES 
{
  DB_SCURRY_OWNER_ID,
  DB_SCURRY_NAME,
  DB_TOTAL_STOLEN_ACORNS,
  DB_WAR_STASH,
  DB_WAR_FLAG,
  DB_PREV_STOLEN_ACORNS
};

enum INVITE_RES {
  DB_INVITE_OWNER, // scurry owner
  DB_INVITE_USER, // inquiring user
  DB_INVITE_MSG, // message id of invite
  DB_INVITE_CHANNEL, // channel slash command was sent in
  DB_INVITE_TIME // time slash command was sent
};

enum SCURRY_RANKS 
{
  SEED_NOT,
  ACORN_SNATCHER,
  SEED_SNIFFER,
  OAKFFICIAL,
  ROYAL_NUT
};

/* Embed Field Indexes */
enum P_TOPIC {
  P_TOPIC_INTRO,
  P_TOPIC_PROGRESSION,
  P_TOPIC_STORY,
  P_TOPIC_SQUIRREL,
  P_TOPIC_CHANGE_COLOR,
  P_TOPIC_STEAL,
  P_TOPIC_SCURRY,
  P_TOPIC_CONJURED_ACORNS,
  P_TOPIC_HELP,
  P_TOPIC_SIZE
};

enum E_TOPIC {
  E_TOPIC_SEASONS,
  E_TOPIC_ACORN_COUNT,
  E_TOPIC_SPRING,
  E_TOPIC_SUMMER,
  E_TOPIC_FALL,
  E_TOPIC_WINTER,
  E_TOPIC_SIZE
};

enum S_TOPIC {
  S_TOPIC_UTILS,
  S_TOPIC_WARS,
  S_TOPIC_AFTERMATH,
  S_TOPIC_RANKS,
  S_TOPIC_SIZE
};

enum STORY_TOPIC {
  STORY_GRASSLANDS,
  STORY_SEEPING_SANDS,
  STORY_NATURE_END,
  STORY_DEATH_GRIP,
  STORY_LAST_ACORN,
  STORY_TOPIC_SIZE
};

enum RULE_TOPICS {
  RULE_RESPECT,
  RULE_CONTENT,
  RULE_BOT,
  RULE_ADVERTISING,
  RULE_WARNING,
  RULE_SIZE
};

enum ITEMS 
{
  ITEM_ACORNS,
  ITEM_ACORN_COUNT,
  ITEM_GOLDEN_ACORN,
  ITEM_WAR_ACORNS,
  ITEM_ENERGY,
  ITEM_HEALTH,
  ITEM_CONJURED_ACORNS,
  ITEM_CATNIP,
  ITEM_SIZE
};

struct sd_file_data items[ITEM_SIZE] = {
  {
    .formal_name = "Acorns",
    .file_path = "items/acorn.png",

    .emoji_name = "acorns",
    .emoji_id = 1045028765392187402
  },
  {
    .formal_name = "Acorn Count",
    .file_path = "items/acorn_count.png",

    .emoji_name = "acorn_count",
    .emoji_id = 1050407923823677502
  },
  {
    .formal_name = "Golden Acorns",
    .file_path = "items/golden_acorn.png",

    .emoji_name = "golden_acorn",
    .emoji_id = 1045032005420728430
  },
  {
    .formal_name = "War Acorns",
    .file_path = "scurry_utils/war_acorns.png",

    .emoji_name = "war_acorns",
    .emoji_id = 1066105193655193640
  },
  {
    .formal_name = "Energy",
    .file_path = "items/energy.png",

    .emoji_name = "energy",
    .emoji_id = 911642184397258752
  },
  {
    .formal_name = "HP",
    .file_path = "items/health.png",

    .emoji_name = "health",
    .emoji_id = 1064579080281923655
  },
  {
    .formal_name = "Conjured Acorn",
    .file_path = "items/conjured_acorn.png",

    .emoji_name = "conjured_acorn",
    .emoji_id = 1102676336289521774
  },
  {
    .formal_name = "Catnip",
    .file_path = "items/catnip.png",

    .emoji_name = "catnip",
    .emoji_id = 1052250368039452732
  }
};

enum ITEM_TYPE {
  TYPE_UNIDENTIFIED,
  TYPE_NO_ACORNS,
  TYPE_ACORN_HANDFUL,
  TYPE_ACORN_MOUTHFUL,
  TYPE_LOST_STASH,
  TYPE_ACORN_SACK,
  TYPE_DIRT_PILE,
  TYPE_ENCOUNTER,
  TYPE_HEALTH_LOSS,
  TYPE_SIZE
};

struct sd_file_data item_types[TYPE_SIZE] = {
  {
    .formal_name = "progenitor",
    .file_path = "item_types/progenitor.png"
  },
  {
    .formal_name = "an Empty Hole",
    .file_path = "item_types/no_acorns.png",

    .emoji_name = "no_acorns",
    .emoji_id = 1044620527223975957
  },
  {
    .formal_name = "a Handful of Acorns",
    .file_path = "item_types/acorn_handful.png",

    .emoji_name = "acorn_handful",
    .emoji_id = 1044620522236944384
  },
  {
    .formal_name = "a Mouthful of Acorns",
    .file_path = "item_types/acorn_mouthful.png",

    .emoji_name = "acorn_mouthful",
    .emoji_id = 1044620523067416627
  },
  {
    .formal_name = "a Lost Stash",
    .file_path = "item_types/lost_stash.png",

    .emoji_name = "lost_stash",
    .emoji_id = 1044620525944705146
  },
  {
    .formal_name = "a Sack of Acorns",
    .file_path = "item_types/acorn_sack.png",

    .emoji_name = "acorn_sack",
    .emoji_id = 1044620524275380286
  },
  {
    .formal_name = "Dirt Pile",
    .file_path = "item_types/dirt_pile.png",

    .emoji_name = "dirt_pile",
    .emoji_id = 1044620525365895168
  },
  {
    .formal_name = "an Encounter",
    .file_path = "item_types/quest_marker.png",

    .emoji_name = "quest_marker",
    .emoji_id = 1046493067500335104
  },
  {
    .formal_name = "HP Lost",
    .file_path = "item_types/broken_health.png",

    .emoji_name = "broken_health",
    .emoji_id = 1064579097956712528
  }

};

enum SQUIRREL_STATS 
{
  STAT_PROFICIENCY,
  STAT_LUCK,
  STAT_STRENGTH,
  STAT_SIZE
};

struct sd_obj_stats stats[STAT_SIZE] = {
  {
    .stat = {
      .formal_name = "Proficiency",
      .file_path = "stats/proficiency.png",
      .description = "*Multiplies acorn "ACORNS" earnings*",

      .emoji_name = "proficiency",
      .emoji_id = 1066084547931947141
    },

    .price_mult = PROFICIENCY_UNIT,
    .value_mult = PROFICIENCY_FACTOR
  },
  {
    .stat = {
      .formal_name = "Luck",
      .file_path = "stats/luck.png",
      .description = "*Multiplies golden acorn "GOLDEN_ACORNS" earnings*",

      .emoji_name = "luck",
      .emoji_id = 1066084537798500372
    },

    .price_mult = LUCK_UNIT,
    .value_mult = LUCK_FACTOR
  },
  {
    .stat = {
      .formal_name = "Strength",
      .file_path = "stats/strength.png",
      .description = "*Adds 5 max "HEALTH" chances*",

      .emoji_name = "strength",
      .emoji_id = 1066084556110831697
    },

    .price_mult = STRENGTH_UNIT,
    .value_mult = STRENGTH_FACTOR
  }
};

enum BUFFS 
{
  BUFF_PROFICIENCY_ACORN,
  BUFF_LUCK_ACORN,
  BUFF_BOOSTED_ACORN,
  BUFFS_SIZE
};

struct sd_file_data enchanted_acorns[BUFFS_SIZE] = {
  {
    .formal_name = "Acorn of Proficiency",
    .file_path = "enchanted_acorns/proficiency_acorn.png",
    .description = "*Doubles acorn earnings and acorn count*",

    .emoji_name = "proficiency_acorn",
    .emoji_id = 1045027184475123813
  },
  {
    .formal_name = "Acorn of Luck",

    .file_path = "enchanted_acorns/luck_acorn.png",
    .description = "*Doubles golden acorn earnings*",

    .emoji_name = "luck_acorn",
    .emoji_id = 1045027183334264872
  },
  {
    .formal_name = "Conjured Acorn Boost",

    .file_path = "enchanted_acorns/boosted_acorn.png",
    .description = "*1.5x squirrel effect*",

    .emoji_name = "boosted_acorn",
    .emoji_id = 1119273554886799450
  }
};

enum BUNNY_STORE
{
  BUNNY_ACORNS,
  BUNNY_GOLDEN_ACORNS,
  BUNNY_HEALTH,
  BUNNY_ENERGY,
  BUNNY_STORE_SIZE
};

enum TYPE_VICTUALS {
  VICTUALS_BLUEBERRY,
  VICTUALS_CHERRY,
  VICTUALS_SEED,
  VICTUALS_SIZE
};

struct sd_file_data victuals[VICTUALS_SIZE] = {
  {
    .formal_name = "Blueberries",
    .file_path = "victuals/blueberry_victuals.png",

    .emoji_name = "blueberry_victuals",
    .emoji_id = 1094282921944305724
  },
  {
    .formal_name = "Cherries",
    .file_path = "victuals/cherry_victuals.png",

    .emoji_name = "cherry_victuals",
    .emoji_id = 1094282923806556251,
  },
  {
    .formal_name = "Seeds",
    .file_path = "victuals/seed_victuals.png",

    .emoji_name = "seed_victuals",
    .emoji_id = 1094282924750274572
  }
};

enum SQUIRREL 
{
  GRAY_SQUIRREL,
  SKELETAL_SQUIRREL,
  SQUIRREL_BOOKIE,
  ANGELIC_SQUIRREL,
  KING_SQUIRREL,
  SQUIRREL_SIZE
};

struct sd_squirrel squirrels[SQUIRREL_SIZE] = {
  {
    .acorn_count_req = GRAY_SQUIRREL_COUNT,
    .boosted_duration = GRAY_BOOST_DURATION,
    
    .squirrel = {
      .formal_name = "Gray Squirrel",
      .file_path = "squirrels/gl_squirrel.png",

      .description = "*Chance to not consume energy*",

      .emoji_name = "gl_squirrel",
      .emoji_id = 1014655900373504130
    }
  },
  {
    .acorn_count_req = SKELETAL_SQUIRREL_COUNT,
    .boosted_duration = SKELETAL_BOOST_DURATION,
    
    .squirrel = {
      .formal_name = "Skeletal Squirrel",
      .file_path = "squirrels/skeletal_squirrel.png",

      .description = "*Increased HP regen*",

      .emoji_name = "skeletal_squirrel",
      .emoji_id = 1072150369599234058
    }
  },
  {
    .acorn_count_req = BOOKIE_SQUIRREL_COUNT,
    .boosted_duration = BOOKIE_BOOST_DURATION,
    
    .squirrel = {
      .formal_name = "Squirrel Bookie",
      .file_path = "squirrels/squirrel_bookie.png",

      .description = "*Increased acorn earnings*",

      .emoji_name = "squirrel_bookie",
      .emoji_id = 1050175322286469250
    }
  },
  {
    .acorn_count_req = ANGELIC_SQUIRREL_COUNT,
    .boosted_duration = ANGELIC_BOOST_DURATION,
    
    .squirrel = {
      .formal_name = "Angelic Squirrel",
      .file_path = "squirrels/angelic_squirrel.png",

      .description = "*Increased lost stash chance*",

      .emoji_name = "angelic_squirrel",
      .emoji_id = 1140690614137139314
    }
  },
  {
    .acorn_count_req = KING_SQUIRREL_COUNT,
    .boosted_duration = KING_BOOST_DURATION,
    
    .squirrel = {
      .formal_name = "King Squirrel",
      .file_path = "squirrels/king_squirrel.png",

      .description = "*Increased acorn count*",

      .emoji_name = "king_squirrel",
      .emoji_id = 1073298108508803113
    }
  }
};

enum BIOME 
{
  GRASSLANDS,
  SEEPING_SANDS,
  NATURE_END,
  DEATH_GRIP,
  LAST_ACORN,
  BIOME_SIZE
};

/* Encounter solutions are ordered by: action, logical, chance */
struct sd_biome biomes[BIOME_SIZE] = {
  { // GRASSLANDS
    .biome_scene_path = "gl_content/grasslands_scene.gif",

    .biome_icon = {
      .formal_name = "Grasslands",
      .file_path = "gl_content/gl_icon.png",

      .emoji_name = "gl_icon",
      .emoji_id = 1007390631016017991
    },

    .intro = "Welcome to the "GRASSLANDS_ICON" **Grasslands**! \n"
        "The biome has luscious green forests, flourishing wildlife, and fertile soils. The land is so rich, sometimes the trees drop golden acorns! \n"
        "The main settlement is Imperium City and has been wreaking havoc on the environment due to political corruption.",

    .sections = (struct sd_biome_section[])
    {
      {
        .section_name = "Enchanted Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Fluffy Unicorn",
            .conflict = "*The unicorn seems friendly... perhaps you could just sneak that acorn from under him?* \n",
            .file_path = "gl_content/encounters/unicorn.png",

            .solutions = {"Aim for the throat!", "Cause a distraction", "Bolt out and hope you scare it"},
            .context = "Inhabited by all sorts of mythical creatures."
          },
          {
            .name = "Burly Pupper",
            .conflict = "*A werewolf has spotted you and begins to chase you!* \n",
            .file_path = "gl_content/encounters/werewolf.png",

            .solutions = {"Aim for the throat!", "Run into a nearby bush", "Run for your life!"},
            .context = "Trees spiral in all sorts of directions, radiating ancient magic."
          },
          {
            .name = "Unique Goldfish",
            .conflict = "*This isn't your everyday goldfish. What's he doin' out of the water?* \n",
            .file_path = "gl_content/encounters/magical_goldfish.png",

            .solutions = {"Eat him!", "Go around him", "Push him back into the water"},
            .context = "Ponds and rivers glow purple with faint reflections of ancient ruins."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Imperium City",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Rat Infestation",
            .conflict = "*Rats! They surely don't like you. Who knows what they could be carrying...* \n",
            .file_path = "gl_content/encounters/city_rat.png",

            .solutions = {"Give him a peace offering", "Back off", "Hug him"},
            .context = "The city is made entirely of white marble."
          },
          {
            .name = "Bold Coyotes",
            .conflict = "*These coyotes keep inching towards you. They're curious but do you want to take that chance?* \n",
            .file_path = "gl_content/encounters/coyote.png",

            .solutions = {"Punch him in the snoot and run up a tree", "Shriek to scare him off", "Take your chances"},
            .context = "Its citizens are known for their curiosity, intellect, and profound fascination with the nearby Enchanted Woods."
          },
          {
            .name = "Rampant Squirrels",
            .conflict = "*Still cute and fluffy- sort of. They dont like other squirrels though.* \n",
            .file_path = "gl_content/encounters/savage_squirrel.png",

            .solutions = {"Offer an acorn", "Wait for him to look the other way", "Try to tame him"},
            .context = "Political corruption has led to pollution, high rodent populations, and agitated wildlife."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Desolate Plains",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Art of Hunting",
            .conflict = "*A camoflauge snare has caught you! It didn't do much, the flimsy thing.* \n",
            .file_path = "gl_content/encounters/bandit_snare.png",

            .solutions = {"Run as fast as you can to break the snare", "Chew through the string", "Wait for another squirrel to help you"},
            .context = "Once farmland that fed the city's residents."
          },
          {
            .name = "Bullseye",
            .conflict = "*You spot a bandit hunter in the corner of your eye as he draws his bow!* \n",
            .file_path = "gl_content/encounters/hunter_detection.png",

            .solutions = { "Throw an acorn", "Lose his sight", "Hide behind a tree"},
            .context = "Now swarms with rebels who are unhappy with the Imperium City's conditions."
          },
          {
            .name = "Suspicious Crops",
            .conflict = "*Unlike the other crops, this one is sticky...* \n",
            .file_path = "gl_content/encounters/poisoned_crop.png",

            .solutions = {"Try a kernel", "Walk away", "Wipe off the poison in hopes there's no trace"},
            .context = "Rebels burn farmland, poison crops, and poach local wildlife."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Witch Swamp",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Is it an Acorn?",
            .conflict = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
            .file_path = "gl_content/encounters/conjured_acorn.png",

            .solutions = {"Investigate the acorn", "Ignore it", "Take the acorn"},
            .context = "Witches were once reputable alchemists in the city."
          },
          {
            .name = "Are you you?",
            .conflict = "*It's a friendly squirrel! Is it?* \n",
            .file_path = "gl_content/encounters/squirrel_doppleganger.png",

            .solutions = {"Indicate you know it's a disguise", "Ignore him", "Shake his paw"},
            .context = "Twisted by the growing corruption, the witches started conducting dark rituals on animals."
          },
          {
            .name = "Poison from Below! Above?",
            .conflict = "*A witch has spotted you and threw a flask!* \n",
            .file_path = "gl_content/encounters/thrown_flask.png",

            .solutions = {"Run at the witch!", "Dodge the flask", "Try to catch the flask"},
            .context = "The witches' practice was outlawed by Imperium City and the witches fled to the swamp as social outcasts."
          }
        },

        .section_size = 3
      }
    },

    .biome_size = 4

  },
  { // SEEPING SANDS
    .biome_scene_path = "sp_content/seeping_sands_scene.gif",

    .biome_icon = {
      .formal_name = "Seeping Sands",
      .file_path = "sp_content/sp_icon.png",

      .emoji_name = "sp_icon",
      .emoji_id = 1042082262566572153
    },

    .intro = "Welcome to the "SEEPING_SANDS_ICON" **Seeping Sands**! \n"
        "The biome is a vast desert wasteland with little to no vegetation that sees frequent sandstorms. "
        "The main settlement here is the Forgotten City whose agriculture malpractice caused the fertile soil to errode.",

    .sections = (struct sd_biome_section[])
    {
      {
        .section_name = "Sandy Shores",
        .encounters = (struct sd_encounter[])
        { 
          { 
            .name = "Desparate Bobcat",
            .conflict = "*There's an acorn next to the feasting bobcat.* \n",
            .file_path = "sp_content/encounters/bobcat.png",

            .solutions = {"Don't look!", "Shake a nearby bush", "Snatch it and hope you don't get eaten"},
            .context = "The desert is also devoid of prey causing predators to become desperate."
          },
          {
            .name = "Sleeping Cobra",
            .conflict = "*You've caught the attention of this cobra!* \n",
            .file_path = "sp_content/encounters/cobra.png",

            .solutions = {"Aim for the neck!", "Flee the scene", "Dodge the lunge"},
            .context = "These areas see a dense population of snakes."
          },
          {
            .name = "Distracting Vibrations",
            .conflict = "*The rattling of the rattlesnake's tail catches your attention and you lock eyes...* \n",
            .file_path = "sp_content/encounters/rattlesnake.png",

            .solutions = {"Aim for the throat!", "Ignore it and scurry away", "Approach it"},
            .context = "Deadly snakes camouflage with the sands."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Forgotten City",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Vicious Land Lobsters",
            .conflict = "*A scorpion finds you and charges to attack you!* \n",
            .file_path = "sp_content/encounters/scorpion.png",

            .solutions = {"Throw an acorn", "Scurry Away!", "Dodge the pincer"},
            .context = "The city is built entirely of sandstone and partially overridden with sand."
          },
          {
            .name = "Unhappy Camels",
            .conflict = "*This camel doesn't look too happy. Watch out for the spit!* \n",
            .file_path = "sp_content/encounters/angry_camel.png",

            .solutions = {"Aim for the face!", "Cause a distraction", "Sneak around"},
            .context = "A once prosperous city for trade went to ruin due to growing internal economic problems."
          },
          {
            .name = "Starving Coyotes",
            .conflict = "*You've accidentally approached a pack of coyotes!* \n",
            .file_path = "sp_content/encounters/coyote.png",

            .solutions = {"Snatch the acorn and run", "Look somewhere else", "Run to the nearby group of people"},
            .context = "The locals have resorted to poaching, leaving little for other animals."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Golden Dunes",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Desert Scavengers",
            .conflict = "*A pack of hyenas are giving you the death stare...* \n",
            .file_path = "sp_content/encounters/hyena.png",

            .solutions = {"Aim for the throat!", "Run to a nearby carcass", "Try to touch one"},
            .context = "The vast golden dunes stretches across the majority of the desert."
          },
          {
            .name = "The Curious Jackal",
            .conflict = "*This jackal doesn't seem threatened nor scared. He's, curious?* \n",
            .file_path = "sp_content/encounters/jackal.png",

            .solutions = {"Hand him an acorn", "Give him head pats", "Ignore him and scurry away"},
            .context = "Animals who don't come in contact with people are often curious, but not all."
          },
          {
            .name = "Desert Puppers",
            .conflict = "*They look a bit hungry. Maybe not a friendly foe...* \n",
            .file_path = "sp_content/encounters/wild_dog.png",

            .solutions = {"Offer an acorn", "Scurry awway!", "Show them to some food"},
            .context = "The whole desert has felt the effects of the impoverished city."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Ancient Wastelands",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Here Comes the Calvary!",
            .conflict = "*A living skeleton? A horse? Better hope it's not that unicorn...* \n",
            .file_path = "sp_content/encounters/skeletal_horse.png",

            .solutions = {"Aim for the throat!", "Cause a distraction", "Bolt out and hope you scare it"},
            .context = "These wastelands are where many ancient battles took place."
          },
          {
            .name = "Squirrel Meets Squirrel",
            .conflict = "*What looks to be a skeletal squirrel makes eye contaact with you... Well? You can't just ignore it!* \n",
            .file_path = "sp_content/encounters/skeletal_squirrel.png",

            .solutions = {"Shake its paw", "Offer an acorn", "Tame it"},
            .context = "Locals believe that these remains were risen by witches."
          }
        },

        .section_size = 2
      },
      {
        .section_name = "Wormhole",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Shifting Doom",
            .conflict = "*A massive worm is shifting through the sands...* \n",
            .file_path = "sp_content/encounters/death_worm.png",

            .solutions = {"Lure and fight it", "Move in the opposite direction", "Run past it"},
            .context = "Massive worms shift in the sands and feel the vibrations of footsteps."
          },
          {
            .name = "Blood-sucking Fiends!",
            .conflict = "*Leeches! Leeches everywhere! Small, but best not let too many latch on!* \n",
            .file_path = "sp_content/encounters/leech.png",

            .solutions = {"Crush them", "Scurry away!", "Run to a nearby carcass"},
            .context = "Leeches accompany the worms in large numbers..."
          }
        },

        .section_size = 2
      }
    },

    .biome_size = 5
  },
  { // NATURE'S END
    .biome_scene_path = "ne_content/nature_end_scene.gif",

    .biome_icon = {
      .formal_name = "Nature's End",
      .file_path = "ne_content/ne_icon.png",
      .emoji_name = "ne_icon",
      .emoji_id = 1071474392783142932
    },

    .intro = "Welcome to "NATURE_END_ICON" **Nature's End**! \n"
        "This chaotic biome is characterized by gloomy skies, heavy and unpredictable rain storms, and earthquakes. It gets its name from the neighboring corruption consuming the wildlife. \n"
        "The main settlement here is Light's Refuge.",

    .sections = (struct sd_biome_section[])
    {
      {
        .section_name = "Light's Refuge",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "The Kind Beast",
            .conflict = "*This bear doesn't seem one bit intmidated! Perhaps he's friendly?* \n",
            .file_path = "ne_content/encounters/bear.png",

            .solutions = {"Give him head pats", "Don't take your chances", "Offer an acorn"},
            .context = "A local oracle resides here to heal the forest."
          },
          {
            .name = "Curious Fox",
            .conflict = "*The fox looks at you and sits expectantly.* \n",
            .file_path = "ne_content/encounters/fox.png",

            .solutions = {"Say \"roll over\" in squirrel", "Give him head pats", "Throw an acorn"},
            .context = "The animals that roam here are very friendly."
          },
          {
            .name = "Santa's Good Boi",
            .conflict = "*Looks like Santa may have lost one! He seems nice...* \n",
            .file_path = "ne_content/encounters/reindeer.png",

            .solutions = {"Give him a random carrot", "Give him chin rubs", "Take him with you"},
            .context = "Although this section of pine forest protected, the oracle is struggling to maintain the light."
          }
        },

        .section_size = 3
      },
      {
        .section_name = "Nature's Dying Light",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Desparate Elk",
            .conflict = "*This elk doesn't appear to be aggressive. Perhaps he's just hungry?* \n",
            .file_path = "ne_content/encounters/elk.png",

            .solutions = {"Offer an acorn", "Lead him to a food source", "Give him head pats"},
            .context = "The border between the protected and corrupt pine forests."
          },
          {
            .name = "Threatened Moose",
            .conflict = "*A defensive moose detects you and charges at you!* \n",
            .file_path = "ne_content/encounters/moose.png",

            .solutions = {"Aim for the throat!", "Climb up a tree", "Run under him"},
            .context = "The presence of the corruption causes these animals to be on edge and untrusting."
          },
          {
            .name = "Spooked Wolf",
            .conflict = "*A twig you stepped on startled a wolf! He's approaching...* \n",
            .file_path = "ne_content/encounters/wolf.png",

            .solutions = {"Aim for throat!", "Run up a tree", "Hide in a bush"},
            .context = "Trees are a mix of pine and corrupt trees and the grass has a purple hue."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Breached Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Collapsed Caves",
            .conflict = "*This bear doesn't seem too happy his home was destroyed...* \n",
            .file_path = "ne_content/encounters/angry_bear.png",

            .solutions = {"Boop his snoot", "Offer an acorn", "Give him a hug"},
            .context = "The effects of the corruption have compromised many of the natural structures."
          },
          {
            .name = "Consumed Elk",
            .conflict = "*Taken by the growing corruption, his mind isn't his own!* \n",
            .file_path = "ne_content/encounters/consumed_elk.png",

            .solutions = {"Aim for the throat!", "Flee into a tree", "Squeal!"},
            .context = "Animals will be left with purple marks when they eat corrupt matter."
          },
          {
            .name = "Corrupt Squirrel",
            .conflict = "*This one doesn't want to shake your paw... You're his snack!* \n",
            .file_path = "ne_content/encounters/corrupt_squirrel.png",

            .solutions = {"Force his paw out", "Outrun him", "Offer an acorn"},
            .context = "Once the animals has been comsumed by the corruption, its mind is no longer its own."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Ancient Ruins",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Sewage Rats",
            .conflict = "*[dry heaves] Those rats STINK! Where have they been, the sewers?* \n",
            .file_path = "ne_content/encounters/city_rat.png",

            .solutions = {"Throw them in water", "Flee their stench", "Hold your breath"},
            .context = "Once a colony of Imperium City, it fell to the corruption roughly 63 years ago."
          },
          {
            .name = "Aggressive Trash Pandas",
            .conflict = "*Is it growling? Is it smiling? Does it want your acorns?* \n",
            .file_path = "ne_content/encounters/raccoon.png",

            .solutions = {"Bonk his head with an acorn", "Throw an acorn and run", "Climb up a building"},
            .context = "While this area remains somewhat intact, many animals reside here to buy themselves time to escape the corruption."
          },
          {
            .name = "Ancient Squirrel",
            .conflict = "*This squirrel has seen a bit... Can he see?* \n",
            .file_path = "ne_content/encounters/skeletal_squirrel.png",

            .solutions = {"Hope he can see your acorn offer", "Shake his paw if he can feel it", "Tame him if he knows who you are"},
            .context = "Not only can the corruption consume the living, but it can also raise the deceased."
          }
        },

        .section_size = 3
      }
    },

    .biome_size = 4
  },
  { // DEATH'S GRIP
    .biome_scene_path = "dg_content/death_grip_scene.gif",

    .biome_icon = {
      .formal_name = "Death's Grip",
      .file_path = "dg_content/dg_icon.png",

      .emoji_name = "dg_icon",
      .emoji_id = 1092903646196412426
    },

    .intro = "Welcome to "DEATH_GRIP_ICON" **Death's Grip**! \n"
        "This biome is a desolate tundra with little life. It gets its name from its naturally unforgiving climate and the growing threat of the corruption. What was once a white and bright tundra is now riddled with necrotic energy and heavy fog. \n"
        "The main settlement here is the Kingdom of Ice.",

    .sections = (struct sd_biome_section[])
    {
      { 
        .section_name = "Boreal Forest",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Fluffy Bunn- uh, Foxes",
            .conflict = "*An arctic fox's head pops above the snow and locks eyes with you.* \n",
            .file_path = "dg_content/encounters/arctic_fox.png",

            .solutions = {"Stare harder", "Keep running", "Wave"},
            .context = "The average height of snow for this area is roughly 3 feet."
          },
          {
            .name = "Sneaky Puppers",
            .conflict = "*You hear snow patting and realize a wolf is stalking you!* \n",
            .file_path = "dg_content/encounters/arctic_wolf.png",

            .solutions = {"Aim for the throat!", "Scurry faster", "Throw an acorn"},
            .context = "The animals are hostile from hunger, no protection from the elements, and the neighboring corruption."
          },
          {
            .name = "Waddle Waddle",
            .conflict = "*This curious penguin approaches you with intrigument.* \n",
            .file_path = "dg_content/encounters/penguin.png",

            .solutions = {"Offer an acorn", "Stop him before he pecks", "Scurry on"},
            .context = "The curious animals will expect something in return for their bravery."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Necrotic Lakes",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Purple Energy",
            .conflict = "*That- that's a squirrel? So dark. So smokey. So... deadly...* \n",
            .file_path = "dg_content/encounters/dark_skeletal_squirrel.png",

            .solutions = {"Squeal!", "Ignore it", "Run up a tree"},
            .context = "These lakes are dense with black mist."
          },
          {
            .name = "Squirrel-o-mancer",
            .conflict = "*Does he raise- tiny fluffballs of doom? Should you find out?* \n",
            .file_path = "dg_content/encounters/necro-squirrel.png",

            .solutions = {"Bonk him on the cabeza with an acorn", "Don't want to know", "Approach"},
            .context = "As you draw closer to the corruption, these corrupt animals develop strategy and critical thinking."
          },
          {
            .name = "Re-foraging",
            .conflict = "*This poor guy was in the unfortunate hands of a necro-squirrel.* \n",
            .file_path = "dg_content/encounters/undead_squirrel.png",

            .solutions = {"Offer an acorn", "Put him out of his misery", "Hug him"},
            .context = "Some were unfortunate enough to be raised back into the hell they wanted to leave."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Kingdom of Ice",
        .encounters = (struct sd_encounter[])
        {
          {
            .name = "Desparate for Warmth",
            .conflict = "*This squirrel has been wandering aimlessly for hours. But he finds a warm companion...* \n",
            .file_path = "dg_content/encounters/frost_squirrel.png",

            .solutions = { "Cough", "Scurry away", "Don't make eye contact"},
            .context = "Built with packed snow and hardened ice supported by boreal beams."
          },
          {
            .name = "Nice Teddy Bear",
            .conflict = "*This polar bear appears to be a bit lonely...* \n",
            .file_path = "dg_content/encounters/polar_bear.png",

            .solutions = {"Offer an acorn", "Snuggle with him", "Let him be"},
            .context = "The animals that manage to survive here are far and few between."
          },
          {
            .name = "Definitely a Fluffy Bunny",
            .conflict = "*He's so cute, small ans squishy you could just-* \n",
            .file_path = "dg_content/encounters/snowshoe_hare.png",

            .solutions = {"Give him head pats", "Call him Squishy", "Bring him home"},
            .context = "There's an unusually large bunny population."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Lost City of Honor",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Bony Teddy Bear",
            .conflict = "*Those claws look sharp and it doesn't look happy...* \n",
            .file_path = "dg_content/encounters/skeletal_bear.png",

            .solutions = {"Aim for the throat!", "Scurry away", "Ride it to where you want to go next"},
            .context = "Closest to the corruption, this city was the first to fall."
          },
          {
            .name = "Future Me?",
            .conflict = "*We bare uncanny resemblance, but it prefers to be alone.* \n",
            .file_path = "dg_content/encounters/skeletal_squirrel.png",

            .solutions = {"Poke with an acorn until it does something", "Offer an acorn", "Tame it"},
            .context = "Some animals here must have been raised by necromancers and their behavior tells us they retain their memories."
          },
          {
            .name = "Not so Fluffy...",
            .conflict = "*Can it see? Can it hear? What do you do?* \n",
            .file_path = "dg_content/encounters/skeletal_wolf.png",

            .solutions = {"Throw an acorn to see if it can feel", "Boop his skeletal snoot", "Squeal to test its hearing"},
            .context = "Necromancers prefer ruins to prevent their rituals from being found."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Death's Valley",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Less Fluffy Bunn- uh, Fox",
            .conflict = "*This fox has been at the mercy of some necromancers... Not friendly.* \n",
            .file_path = "dg_content/encounters/consumed_fox.png",

            .solutions = {"Offer an acorn for its troubles", "Scurry away", "Dance to lighten its mood"},
            .context = "Animals here are used by necromancers to do their biddings."
          },
          {
            .name = "Nature's Dilemma",
            .conflict = "*So cute but scary at the same!* \n",
            .file_path = "dg_content/encounters/consumed_hare.png",

            .solutions = {"Stare in confusion", "Don't look in its eyes!", "Attempt to pat its head"},
            .context = "Even the cutest of animals fall to the corruption."
          },
          {
            .name = "Angry Teddy Bear",
            .conflict = "*This polar bear doesn't look too happy about his situation...* \n",
            .file_path = "dg_content/encounters/consumed_polar_bear.png",

            .solutions = {"Appease with an acorn", "Scurry away", "Pretend like he isn't there"},
            .context = "What few animals manage to survive here are in a perpetual state of fear."
          }
        },

        .section_size = 3
      }
    },

    .biome_size = 5
  },
  { // THE LAST ACORN
    .biome_scene_path = "la_content/la_scene.gif",

    .biome_icon = {
      .formal_name = "The Last Acorn",
      .file_path = "la_content/la_icon.png",

      .emoji_name = "la_icon",
      .emoji_id = 1105304340400451645
    },

    .intro = "Welcome to the "LAST_ACORN_ICON" **The Last Acorn**! \n"
        "This biome looks identical the Grasslands, but it's characterized by dense purple fog, humming dark energy, blood of the sacrificed coursing through the dirt, and crazed squirrels who once inhabited the area. Its name came from the squirrel colonies because squirrels are lured in by conjured acorns and it's the last acorn they ever see. \n"
        "There is no settlement here.",

    .sections = (struct sd_biome_section[])
    {
      { 
        .section_name = "Weeping Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Dark Pupper Energy",
            .conflict = "*This pupper is different from the rest... he's riddled with dark energy!* \n",
            .file_path = "la_content/encounters/wolf_familiar.png",

            .solutions = {"Throw a golden acorn", "Give him a bone", "Touch it"},
            .context = "These animals are born and raised by the corruption."
          },
          {
            .name = "Fox Zoomies",
            .conflict = "*The foxes notices you, locks eyes with you, pupils grow large...* \n",
            .file_path = "la_content/encounters/fox_familiar.png",
            
            .solutions = {"Aim for the throat", "Make him fetch an acorn", "Give the death stare"},
            .context = "The corruption trains these familiars to be unsuspectingly deadly predators."
          },
          {
            .name = "The Less Fortunate",
            .conflict = "*Was once a happy squirrel, but was unfortunate enough to stumble upon this corruption...* \n",
            .file_path = "la_content/encounters/possessed_squirrel.png",
            
            .solutions = {"Offer an acorn", "Let him be", "Try to comfort him"},
            .context = "This squirrel must have eaten a seemingly normal acorn..."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Shed of Light",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Harmless Fluff Ball",
            .conflict = "*What a lonely little hare! He starts to approach you...* \n",
            .file_path = "la_content/encounters/woodland_hare.png",
            
            .solutions = {"Keep him", "Give him a carrot", "Give him head pats"},
            .context = "The only spot the corruption didn't take for unknown reasons."
          },
          {
            .name = "Happy Elk",
            .conflict = "*This elk is happy. He's surrounded by food and other elk!* \n",
            .file_path = "la_content/encounters/elk.png",
            
            .solutions = {"Offer an acorn", "Admire the scene", "Try to saddle him"},
            .context = "Animals here are happier than those in the oracle's protection."
          },
          {
            .name = "Snoozing Bear",
            .conflict = "*This bear seems content in his slumber. But there's an acorn behind him!* \n",
            .file_path = "la_content/encounters/bear.png",
            
            .solutions = {"Throw an acorn to provoke him", "Sneak around him", "Cause a scene"},
            .context = "The oracle has been seeking answers as to what protects this little area."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Haunted Pasture",
        .encounters = (struct sd_encounter[])
        {
          {
            .name = "The Acorn Dilemma",
            .conflict = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
            .file_path = "la_content/encounters/conjured_acorn.png",
            
            .solutions = {"Throw it at the nearby cultist", "Don't trust it", "Try to crack it open"},
            .context = "Illusions are created by the witches to trap unsuspecting animals."
          },
          {
            .name = "Random Goldfish",
            .conflict = "*This goldfish is flopping around on the ground far away from any body of water...* \n",
            .file_path = "la_content/encounters/conjured_goldfish.png",
            
            .solutions = {"Bring it to a body of water", "Scurry away", "Test sniff"},
            .context = "Witches here and can recreate your past memories as if they were reality."
          },
          {
            .name = "It's a Normal Squirrel, Right?",
            .conflict = "*A suspicious looking squirrel is feasting on some victuals.* \n",
            .file_path = "la_content/encounters/squirrel_doppleganger.png",
            
            .solutions = {"Throw an acorn", "Scurry away", "Grab its attention"},
            .context = "The necromancers will use the powers of the corruption to revive the deceased and the witches disguise them."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Plain of Embers",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Flaming Equine",
            .conflict = "*It looks peaceful despite its appearance.* \n",
            .file_path = "la_content/encounters/nightmare.png",
            
            .solutions = {"Reach for head pats", "Don't trust it", "Try to saddle it"},
            .context = "These creatures are neither animal nor familiar, but hellspawn."
          },
          {
            .name = "A fellow Bunny?",
            .conflict = "*But he's just A BUNNY!* \n",
            .file_path = "la_content/encounters/molten_bunny.png",
            
            .solutions = {"Let him be", "Give him head pats", "Take him with you"},
            .context = "The grass is hot to the touch and releases sparks."
          },
          {
            .name = "To Touch Or Not To Touch",
            .conflict = "*It moves without a trace! But it's so tempting to touch it...* \n",
            .file_path = "la_content/encounters/living_dark_flame.png",
            
            .solutions = {"Try to touch it", "Try to blow it out", "Follow it"},
            .context = "Hellspawn come from a portal that connects Earth to Hell."
          }
        },

        .section_size = 3
      },
      { 
        .section_name = "Death's Locus",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Squirrel Power",
            .conflict = "*This squirrel has learned to wield the corruptive powers!* \n",
            .file_path = "la_content/encounters/dark_squirrel_mage.png",
            
            .solutions = {"Throw an acorn", "Run", "Negotiate peace"},
            .context = "These squirrels draw off the corruption's power with the intent of dominating the world."
          },
          {
            .name = "Squirrel Fanatic",
            .conflict = "*Brainwashed by the corruptive forces, this squirrel isn't natural!* \n",
            .file_path = "la_content/encounters/squirrel_cultist.png",
            
            .solutions = {"Throw an acorn at it", "Scurry away", "Does it like acorns?"},
            .context = "Any human or animal that enters the corruption never makes it out in one piece."
          },
          {
            .name = "The Talking Squirrel",
            .conflict = "*It speaks but its mouth doesn't move? What is it saying?* \n",
            .file_path = "la_content/encounters/familiar_skeletal_squirrel.png",
            
            .solutions = {"Touch it", "Wave", "Find the voice"},
            .context = "Mind control can become so powerful that some animals can chant. But what are they saying?"
          }
        },

        .section_size = 3
      }
    },

    .biome_size = 5
  }
};