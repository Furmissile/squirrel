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
  // skip user id
  DB_PROFICIENCY_LV = 16,
  DB_STRENGTH_LV,
  DB_LUCK_LV,
  // skip user id
  DB_PROFICIENCY_ACORN = 20,
  DB_LUCK_ACORN,
  DB_STRENGTH_ACORN,
  DB_ENDURANCE_ACORN,
  DB_BOOSTED_ACORN
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
  P_TOPIC_SQUIRREL,
  P_TOPIC_CHANGE_COLOR,
  P_TOPIC_STEAL,
  P_TOPIC_SCURRY,
  P_TOPIC_CONJURED_ACORNS,
  P_TOPIC_VENGENCE_MODE,
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
  STORY_ORIGINS,
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
  TYPE_ACORN_SACK,
  TYPE_DIRT_PILE,
  TYPE_LOST_STASH,
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
    .formal_name = "a Lost Stash",
    .file_path = "item_types/lost_stash.png",

    .emoji_name = "lost_stash",
    .emoji_id = 1044620525944705146
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
  BUFF_STRENGTH_ACORN,
  BUFF_ENDURANCE_ACORN,
  BUFF_BOOSTED_ACORN,
  BUFFS_SIZE
};

struct sd_file_data enchanted_acorns[BUFFS_SIZE] = {
  {
    .formal_name = "Acorn of Proficiency",
    .file_path = "enchanted_acorns/proficiency_acorn.png",
    .description = "*Doubles proficiency stat*",

    .emoji_name = "proficiency_acorn",
    .emoji_id = 1045027184475123813
  },
  {
    .formal_name = "Acorn of Luck",

    .file_path = "enchanted_acorns/luck_acorn.png",
    .description = "*Doubles luck stat*",

    .emoji_name = "luck_acorn",
    .emoji_id = 1045027183334264872
  },
  {
    .formal_name = "Acorn of Strength",

    .file_path = "enchanted_acorns/healing_acorn.png",
    .description = "*Refills some of HP*",

    .emoji_name = "strength_acorn",
    .emoji_id = 1045027180549255208
  },
  {
    .formal_name = "Acorn of Endurance",

    .file_path = "enchanted_acorns/endurance_acorn.png",
    .description = "*Refills some of energy*",

    .emoji_name = "endurance_acorn",
    .emoji_id = 1045027181996286022
  },
  {
    .formal_name = "Conjured Acorn Boost",

    .file_path = "enchanted_acorns/boosted_acorn.png",
    .description = "*Increases squirrel effect by half*",

    .emoji_name = "boosted_acorn",
    .emoji_id = 1119273554886799450
  }
};

enum SCURRY_ITEMS
{
  SCURRY_ITEM_WAR_ACORNS,
  SCURRY_ITEM_SIZE
};

struct sd_file_data scurry_items[SCURRY_ITEM_SIZE] = {
  {
    .formal_name = "War Acorns",
    .file_path = "scurry_utils/war_acorns.png",

    .emoji_name = "war_acorns",
    .emoji_id = 1066105193655193640
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
  KING_SQUIRREL,
  ANGELIC_SQUIRREL,
  SQUIRREL_SIZE
};

struct sd_squirrel squirrels[SQUIRREL_SIZE] = {
  {
    .acorn_count_req = GRAY_SQUIRREL_COUNT, // base squirrel
    
    .squirrel = {
      .formal_name = "Gray Squirrel",
      .file_path = "squirrels/gl_squirrel.png",

      .description = "*Chance to not consume energy*",

      .emoji_name = "gl_squirrel",
      .emoji_id = 1014655900373504130
    },

    .evo_squirrel  = {
      .formal_name = "Gray Squirrel Evolve",
      .file_path = "evo_squirrels/gl_squirrel_evolve.png",

      .emoji_name = "gl_squirrel_evolve",
      .emoji_id = 1006206208333586493
    }
  },
  {
    .acorn_count_req = SKELETAL_SQUIRREL_COUNT,
    
    .squirrel = {
      .formal_name = "Skeletal Squirrel",
      .file_path = "squirrels/skeletal_squirrel.png",

      .description = "*Doubles health regen*",

      .emoji_name = "skeletal_squirrel",
      .emoji_id = 1072150369599234058
    },

    .evo_squirrel  = {
      .formal_name = "Skeletal Squirrel Evolve",
      .file_path = "evo_squirrel/skeletal_squirrel_evolve.png",

      .emoji_name = "skeletal_squirrel_evolve",
      .emoji_id = 1072150653671047219
    }
  },
  {
    .acorn_count_req = BOOKIE_SQUIRREL_COUNT, // base squirrel
    
    .squirrel = {
      .formal_name = "Squirrel Bookie",
      .file_path = "squirrels/squirrel_bookie.png",

      .description = "*Doubles proficiency stat*",

      .emoji_name = "squirrel_bookie",
      .emoji_id = 1050175322286469250
    },

    .evo_squirrel  = {
      .formal_name = "Squirrel Bookie Evolve",
      .file_path = "evo_squirrel/squirrel_bookie_evolve.png",

      .emoji_name = "squirrel_bookie_evolve",
      .emoji_id = 1072150655281659954
    }
  },
  {
    .acorn_count_req = KING_SQUIRREL_COUNT, // base squirrel
    
    .squirrel = {
      .formal_name = "King Squirrel",
      .file_path = "squirrels/king_squirrel.png",

      .description = "*Doubles acorn count*",

      .emoji_name = "king_squirrel",
      .emoji_id = 1073298108508803113
    },

    .evo_squirrel  = {
      .formal_name = "King Squirrel Evolve",
      .file_path = "evo_squirrel/king_squirrel_evolve.png",

      .emoji_name = "king_squirrel_evolve",
      .emoji_id = 1073298727017648219
    }
  },
  {
    .acorn_count_req = ANGELIC_SQUIRREL_COUNT, // base squirrel
    
    .squirrel = {
      .formal_name = "Angelic Squirrel",
      .file_path = "squirrels/angelic_squirrel.png",

      .description = "*Doubles luck stat*",

      .emoji_name = "angelic_squirrel",
      .emoji_id = 1140690614137139314
    },

    .evo_squirrel  = {
      .formal_name = "Angelic Squirrel Evolve",
      .file_path = "evo_squirrel/angelic_squirrel_evolve.png",

      .emoji_name = "angelic_squirrel_evolve",
      .emoji_id = 1140690663361490994
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
      .file_path = "gl_content/grasslands_icon.png",

      .emoji_name = "gl_icon",
      .emoji_id = 1007390631016017991
    },

    .encounters = (struct sd_encounter[])
    {
      {
        .name = "Enchanted Woods: Fluffy Unicorn",
        .conflict = "*The unicorn seems friendly... perhaps you could just sneak that acorn from under him?* \n",
        .file_path = "gl_content/encounters/unicorn.png",

        .solutions = {"Aim for the throat!", "Cause a distraction", "Bolt out and hope you scare it"}
      },
      {
        .name = "Enchanted Woods: Burly Pupper",
        .conflict = "*A werewolf has spotted you and begins to chase you!* \n",
        .file_path = "gl_content/encounters/werewolf.png",

        .solutions = {"Aim for the throat!", "Run into a nearby bush", "Run for your life!"}
      },
      {
        .name = "Enchanted Woods: Unique Goldfish",
        .conflict = "*This isn't your everyday goldfish. What's he doin' out of the water?* \n",
        .file_path = "gl_content/encounters/magical_goldfish.png",

        .solutions = {"Eat him!", "Go around him", "Push him back into the water"}
      },
      {
        .name = "Imperium City: Rat Infestation",
        .conflict = "*Rats! They surely don't like you. Who knows what they could be carrying...* \n",
        .file_path = "gl_content/encounters/city_rat.png",

        .solutions = {"Give him a peace offering", "Back off", "Hug him"}
      },
      {
        .name = "Imperium City: Bold Coyotes",
        .conflict = "*These coyotes keep inching towards you. They're curious but do you want to take that chance?* \n",
        .file_path = "gl_content/encounters/coyote.png",

        .solutions = {"Punch him in the snoot and run up a tree", "Shriek to scare him off", "Take your chances"}
      },
      {
        .name = "Imperium City: Rampant Squirrels",
        .conflict = "*Still cute and fluffy- sort of. They dont like other squirrels though.* \n",
        .file_path = "gl_content/encounters/savage_squirrel.png",

        .solutions = {"Offer an acorn", "Wait for him to look the other way", "Try to tame him"}
      },
      {
        .name = "Desolate Plains: Art of Hunting",
        .conflict = "*A camoflauge snare has caught you! It didn't do much, the flimsy thing.* \n",
        .file_path = "gl_content/encounters/bandit_snare.png",

        .solutions = {"Run as fast as you can to break the snare", "Chew through the string", "Wait for another squirrel to help you"}
      },
      {
        .name = "Desolate Plains: Bullseye",
        .conflict = "*You spot a bandit hunter in the corner of your eye as he draws his bow!* \n",
        .file_path = "gl_content/encounters/hunter_detection.png",

        .solutions = { "Throw an acorn", "Lose his sight", "Hide behind a tree"}
      },
      {
        .name = "Desolate Plains: Suspicious Crops",
        .conflict = "*Unlike the other crops, this one is sticky...* \n",
        .file_path = "gl_content/encounters/poisoned_crop.png",

        .solutions = {"Try a kernel", "Walk away", "Wipe off the poison in hopes there's no trace"}
      },
      {
        .name = "Witch Swamp: Is it an Acorn?",
        .conflict = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
        .file_path = "gl_content/encounters/conjured_acorn.png",

        .solutions = {"Investigate the acorn", "Ignore it", "Take the acorn"}
      },
      {
        .name = "Witch Swamp: Are you you?",
        .conflict = "*It's a friendly squirrel! Is it?* \n",
        .file_path = "gl_content/encounters/squirrel_doppleganger.png",

        .solutions = {"Indicate you know it's a disguise", "Ignore him", "Shake his paw"}
      },
      {
        .name = "Witch Swamp: Poison from Below! Above?",
        .conflict = "*A witch has spotted you and threw a flask!* \n",
        .file_path = "gl_content/encounters/thrown_flask.png",

        .solutions = {"Run at the witch!", "Dodge the flask", "Try to catch the flask"}
      }
    },

    .encounter_size = 12
  },
  { // SEEPING SANDS
    .biome_scene_path = "sp_content/seeping_sands_scene.gif",

    .biome_icon = {
      .formal_name = "Seeping Sands",
      .file_path = "sp_content/sp_icon.png",

      .emoji_name = "sp_icon",
      .emoji_id = 1042082262566572153
    },

    .encounters = (struct sd_encounter[]) {
      {
        .name = "Sandy Shores: Desparate Bobcat",
        .conflict = "*There's an acorn next to the feasting bobcat.* \n",
        .file_path = "sp_content/encounters/bobcat.png",

        .solutions = {"Don't look!", "Shake a nearby bush", "Snatch it and hope you don't get eaten"}
      },
      {
        .name = "Sandy Shores: Sleeping Cobra",
        .conflict = "*You've caught the attention of this cobra!* \n",
        .file_path = "sp_content/encounters/cobra.png",

        .solutions = {"Aim for the neck!", "Flee the scene", "Dodge the lunge"}
      },
      {
        .name = "Sandy Shores: Distracting Vibrations",
        .conflict = "*The rattling of the rattlesnake's tail catches your attention and you lock eyes...* \n",
        .file_path = "sp_content/encounters/rattlesnake.png",

        .solutions = {"Aim for the throat!", "Ignore it and scurry away", "Approach it"}
      },
      {
        .name = "Forgotten City: Vicious Land Lobsters",
        .conflict = "*A scorpion finds you and charges to attack you!* \n",
        .file_path = "sp_content/encounters/scorpion.png",

        .solutions = {"Throw an acorn", "Scurry Away!", "Dodge the pincer"},
      },
      {
        .name = "Forgotten City: Unhappy Camels",
        .conflict = "*This camel doesn't look too happy. Watch out for the spit!* \n",
        .file_path = "sp_content/encounters/angry_camel.png",

        .solutions = {"Aim for the face!", "Cause a distraction", "Sneak around"}
      },
      {
        .name = "Forgotten City: Starving Coyotes",
        .conflict = "*You've accidentally approached a pack of coyotes!* \n",
        .file_path = "sp_content/encounters/coyote.png",

        .solutions = {"Snatch the acorn and run", "Look somewhere else", "Run to the nearby group of people"}
      },
      {
        .name = "Golden Dunes: Desert Scavengers",
        .conflict = "*A pack of hyenas are giving you the death stare...* \n",
        .file_path = "sp_content/encounters/hyena.png",

        .solutions = {"Aim for the throat!", "Run to a nearby carcass", "Try to touch one"}
      },
      {
        .name = "Golden Dunes: The Curious Jackal",
        .conflict = "*This jackal doesn't seem threatened nor scared. He's, curious?* \n",
        .file_path = "sp_content/encounters/jackal.png",

        .solutions = {"Hand him an acorn", "Give him head pats", "Ignore him and scurry away"}
      },
      {
        .name = "Golden Dunes: Desert Puppers",
        .conflict = "*They look a bit hungry. Maybe not a friendly foe...* \n",
        .file_path = "sp_content/encounters/wild_dog.png",

        .solutions = {"Offer an acorn", "Scurry awway!", "Show them to some food"}
      },
      {
        .name = "Ancient Wastelands: Here Comes the Calvary!",
        .conflict = "*A living skeleton? A horse? Better hope it's not that unicorn...* \n",
        .file_path = "sp_content/encounters/skeletal_horse.png",

        .solutions = {"Aim for the throat!", "Cause a distraction", "Bolt out and hope you scare it"}
      },
      {
        .name = "Ancient Wastelands: Squirrel Meets Squirrel",
        .conflict = "*What looks to be a skeletal squirrel makes eye contaact with you... Well? You can't just ignore it!* \n",
        .file_path = "sp_content/encounters/skeletal_squirrel.png",

        .solutions = {"Shake its paw", "Offer an acorn", "Tame it"}
      },
      {
        .name = "Wormhole: Shifting Doom",
        .conflict = "*A massive worm is shifting through the sands...* \n",
        .file_path = "sp_content/encounters/death_worm.png",

        .solutions = {"Lure and fight it", "Move in the opposite direction", "Run past it"}
      },
      {
        .name = "Wormhole: Blood-sucking Fiends!",
        .conflict = "*Leeches! Leeches everywhere! Small, but best not let too many latch on!* \n",
        .file_path = "sp_content/encounters/leech.png",

        .solutions = {"Crush them", "Scurry away!", "Run to a nearby carcass"}
      }
    },

    .encounter_size = 13
  },
  { // NATURE'S END
    .biome_scene_path = "ne_content/nature_end_scene.gif",

    .biome_icon = {
      .formal_name = "Nature's End",
      .file_path = "ne_content/ne_icon.png",
      .emoji_name = "ne_icon",
      .emoji_id = 1071474392783142932
    },

    .encounters = (struct sd_encounter[]) {
      {
        .name = "Light's Refuge: The Kind Beast",
        .conflict = "*This bear doesn't seem one bit intmidated! Perhaps he's friendly?* \n",
        .file_path = "ne_content/encounters/bear.png",

        .solutions = {"Give him head pats", "Don't take your chances", "Offer an acorn"}
      },
      {
        .name = "Light's Refuge: Curious Fox",
        .conflict = "*The fox looks at you and sits expectantly.* \n",
        .file_path = "ne_content/encounters/fox.png",

        .solutions = {"Say \"roll over\" in squirrel", "Give him head pats", "Throw an acorn"}
      },
      {
        .name = "Light's Refuge: Santa's Good Boi",
        .conflict = "*Looks like Santa may have lost one! He seems nice...* \n",
        .file_path = "ne_content/encounters/reindeer.png",

        .solutions = {"Give him a random carrot", "Give him chin rubs", "Take him with you"}
      },
      {
        .name = "Nature's Dying Light: Desparate Elk",
        .conflict = "*This elk doesn't appear to be aggressive. Perhaps he's just hungry?* \n",
        .file_path = "ne_content/encounters/elk.png",

        .solutions = {"Offer an acorn", "Lead him to a food source", "Give him head pats"}
      },
      {
        .name = "Nature's Dying Light: Threatened Moose",
        .conflict = "*A defensive moose detects you and charges at you!* \n",
        .file_path = "ne_content/encounters/moose.png",

        .solutions = {"Aim for the throat!", "Climb up a tree", "Run under him"}
      },
      {
        .name = "Nature's Dying Light: Spooked Wolf",
        .conflict = "*A twig you stepped on startled a wolf! He's approaching...* \n",
        .file_path = "ne_content/encounters/wolf.png",

        .solutions = {"Aim for throat!", "Run up a tree", "Hide in a bush"}
      },
      {
        .name = "Breached Woods: Collapsed Caves",
        .conflict = "*This bear doesn't seem too happy his home was destroyed...* \n",
        .file_path = "ne_content/encounters/angry_bear.png",

        .solutions = {"Boop his snoot", "Offer an acorn", "Give him a hug"}
      },
      {
        .name = "Breached Woods: Consumed Elk",
        .conflict = "*Taken by the growing corruption, his mind isn't his own!* \n",
        .file_path = "ne_content/encounters/consumed_elk.png",

        .solutions = {"Aim for the throat!", "Flee into a tree", "Squeal!"}
      },
      {
        .name = "Breached Woods: Corrupt Squirrel",
        .conflict = "*This one doesn't want to shake your paw... You're his snack!* \n",
        .file_path = "ne_content/encounters/corrupt_squirrel.png",

        .solutions = {"Force his paw out", "Outrun him", "Offer an acorn"}
      },
      {
        .name = "Ruins of Luxos: Sewage Rats",
        .conflict = "*[dry heaves] Those rats STINK! Where have they been, the sewers?* \n",
        .file_path = "ne_content/encounters/city_rat.png",

        .solutions = {"Throw them in water", "Flee their stench", "Hold your breath"}
      },
      {
        .name = "Ruins of Luxos: Aggressive Trash Pandas",
        .conflict = "*Is it growling? Is it smiling? Does it want your acorns?* \n",
        .file_path = "ne_content/encounters/raccoon.png",

        .solutions = {"Bonk his head with an acorn", "Throw an acorn and run", "Climb up a building"}
      },
      {
        .name = "Ruins of Luxos: Ancient Squirrel",
        .conflict = "*This squirrel has seen a bit... Can he see?* \n",
        .file_path = "ne_content/encounters/skeletal_squirrel.png",

        .solutions = {"Hope he can see your acorn offer", "Shake his paw if he can feel it", "Tame him if he knows who you are"}
      }
    },

    .encounter_size = 12
  },
  { // DEATH'S GRIP
    .biome_scene_path = "dg_content/death_grip_scene.gif",

    .biome_icon = {
      .formal_name = "Death's Grip",
      .file_path = "dg_content/dg_icon.png",

      .emoji_name = "dg_icon",
      .emoji_id = 1092903646196412426
    },

    .encounters = (struct sd_encounter[]) {
      {
        .name = "Boreal Forest: Fluffy Bunn- uh, Foxes",
        .conflict = "*An arctic fox's head pops above the snow and locks eyes with you.* \n",
        .file_path = "dg_content/encounters/arctic_fox.png",

        .solutions = {"Stare harder", "Keep running", "Wave"}
      },
      {
        .name = "Boreal Forest: Sneaky Puppers",
        .conflict = "*You hear snow patting and realize a wolf is after you!* \n",
        .file_path = "dg_content/encounters/arctic_wolf.png",

        .solutions = {"Aim for the throat!", "Scurry faster", "Throw an acorn"}
      },
      {
        .name = "Boreal Forest: Waddle Waddle",
        .conflict = "*This curious penguin approaches you with intrigument.* \n",
        .file_path = "dg_content/encounters/penguin.png",

        .solutions = {"Offer an acorn", "Stop him before he pecks", "Scurry on"}
      },
      {
        .name = "Necrotic Lakes: Purple Energy",
        .conflict = "*That- that's a squirrel? So dark. So smokey. So... deadly...* \n",
        .file_path = "dg_content/encounters/dark_skeletal_squirrel.png",

        .solutions = {"Squeal!", "Ignore it", "Run up a tree"}
      },
      {
        .name = "Necrotic Lakes: Squirrel-o-mancer",
        .conflict = "*Does he raise- tiny fluffballs of doom? Should you find out?* \n",
        .file_path = "dg_content/encounters/necro-squirrel.png",

        .solutions = {"Bonk him on the cabeza with an acorn", "Don't want to know", "Approach"}
      },
      {
        .name = "Necrotic Lakes: Re-foraging",
        .conflict = "*This poor guy was in the unfortunate hands of a necro-squirrel.* \n",
        .file_path = "dg_content/encounters/undead_squirrel.png",

        .solutions = {"Offer an acorn", "Put him out of his misery", "Hug him"}
      },
      {
        .name = "Kingdom of Ice: Desparate for Warmth",
        .conflict = "*This squirrel has been wandering aimlessly for hours. But he finds a warm companion...* \n",
        .file_path = "dg_content/encounters/frost_squirrel.png",

        .solutions = { "Cough", "Scurry away", "Don't make eye contact"}
      },
      {
        .name = "Kingdom of Ice: Nice Teddy Bear",
        .conflict = "*This polar bear appears to be a bit lonely...* \n",
        .file_path = "dg_content/encounters/polar_bear.png",

        .solutions = {"Offer an acorn", "Snuggle with him", "Let him be"}
      },
      {
        .name = "Kingdom of Ice: Definitely a Fluffy Bunny",
        .conflict = "*He's so cute, small ans squishy you could just-* \n",
        .file_path = "dg_content/encounters/snowshoe_hare.png",

        .solutions = {"Give him head pats", "Call him Squishy", "Bring him home"}
      },
      {
        .name = "Lost City of Honor: Bony Teddy Bear",
        .conflict = "*Those claws look sharp and it doesn't look happy...* \n",
        .file_path = "dg_content/encounters/skeletal_bear.png",

        .solutions = {"Aim for the throat!", "Scurry away", "Ride it to where you want to go next"}
      },
      {
        .name = "Lost City of Honor: Future Me?",
        .conflict = "*We bare uncanny resemblance, but it prefers to be alone.* \n",
        .file_path = "dg_content/encounters/skeletal_squirrel.png",

        .solutions = {"Poke with an acorn until it does something", "Offer an acorn", "Tame it"}
      },
      {
        .name = "Lost City of Honor: Not so Fluffy...",
        .conflict = "*Can it see? Can it hear? What do you do?* \n",
        .file_path = "dg_content/encounters/skeletal_wolf.png",

        .solutions = {"Throw an acorn to see if it can feel", "Boop his skeletal snoot", "Squeal to test its hearing"}
      },
      {
        .name = "Death's Valley: Less Fluffy Bunn- uh, Fox",
        .conflict = "*This fox has been at the mercy of some necromancers... Not friendly.* \n",
        .file_path = "dg_content/encounters/consumed_fox.png",

        .solutions = {"Offer an acorn for its troubles", "Scurry away", "Dance to lighten its mood"}
      },
      {
        .name = "Death's Valley: Nature's Dilemma",
        .conflict = "*So cute but scary at the same!* \n",
        .file_path = "dg_content/encounters/consumed_hare.png",

        .solutions = {"Stare in confusion", "Don't look in its eyes!", "Attempt to pat its head"}
      },
      {
        .name = "Death's Valley: Angry Teddy Bear",
        .conflict = "*This polar bear doesn't look too happy about his situation...* \n",
        .file_path = "dg_content/encounters/consumed_polar_bear.png",

        .solutions = {"Appease with an acorn", "Scurry away", "Pretend like he isn't there"}
      }
    },

    .encounter_size = 15
  },
  { // THE LAST ACORN
    .biome_scene_path = "la_content/la_scene.gif",

    .biome_icon = {
      .formal_name = "The Last Acorn",
      .file_path = "la_content/la_icon.png",

      .emoji_name = "la_icon",
      .emoji_id = 1105304340400451645
    },

    .encounters = (struct sd_encounter[]) {
      {
        .name = "Creeping Woods: Dark Pupper Energy",
        .conflict = "*This pupper is different from the rest... he's riddled with dark energy!* \n",
        .file_path = "la_content/encounters/wolf_familiar.png",

        .solutions = {"Throw a golden acorn", "Give him a bone", "Touch it"}
      },
      {
        .name = "Creeping Woods: Fox Zoomies",
        .conflict = "*The foxes notices you, locks eyes with you, pupils grow large...* \n",
        .file_path = "la_content/encounters/fox_familiar.png",
        
        .solutions = {"Aim for the throat", "Make him fetch an acorn", "Give the death stare"}
      },
      {
        .name = "Creeping Woods: The Talking Squirrel",
        .conflict = "*It speaks but its mouth doesn't move? What is it saying?* \n",
        .file_path = "la_content/encounters/familiar_skeletal_squirrel.png",
        
        .solutions = {"Touch it", "Wave", "Find the voice"}
      },
      {
        .name = "Shed of Light: Harmless Fluff Ball",
        .conflict = "*What a lonely little hare! He starts to approach you...* \n",
        .file_path = "la_content/encounters/woodland_hare.png",
        
        .solutions = {"Keep him", "Give him a carrot", "Give him head pats"}
      },
      {
        .name = "Shed of Light: Happy Elk",
        .conflict = "*This elk is happy. He's surrounded by food and other elk!* \n",
        .file_path = "la_content/encounters/elk.png",
        
        .solutions = {"Offer an acorn", "Admire the scene", "Try to saddle him"}
      },
      {
        .name = "Shed of Light: Snoozing Bear",
        .conflict = "*This bear seems content in his slumber. But there's an acorn behind him!* \n",
        .file_path = "la_content/encounters/bear.png",
        
        .solutions = {"Throw an acorn to provoke him", "Sneak around him", "Cause a scene"}
      },
      {
        .name = "Haunted Pasture: The Acorn Dilemma",
        .conflict = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
        .file_path = "la_content/encounters/conjured_acorn.png",
        
        .solutions = {"Throw it at the nearby cultist", "Don't trust it", "Try to crack it open"}
      },
      {
        .name = "Haunted Pasture: Random Goldfish",
        .conflict = "*This goldfish is flopping around on the ground far away from any body of water...* \n",
        .file_path = "la_content/encounters/conjured_goldfish.png",
        
        .solutions = {"Bring it to a body of water", "Scurry away", "Test sniff"}
      },
      {
        .name = "Haunted Pasture: It's a Normal Squirrel, Right?",
        .conflict = "*A suspicious looking squirrel is feasting on some victuals.* \n",
        .file_path = "la_content/encounters/squirrel_doppleganger.png",
        
        .solutions = {"Throw an acorn", "Scurry away", "Grab its attention"}
      },
      {
        .name = "Plain of Embers: Flaming Equine",
        .conflict = "*A spawn of Hell... It looks peaceful despite its appearance.* \n",
        .file_path = "la_content/encounters/nightmare.png",
        
        .solutions = {"Reach for head pats", "Don't trust it", "Try to saddle it"}
      },
      {
        .name = "Plain of Embers: A fellow Bunny?",
        .conflict = "*A spawn of Hell... But he's just A BUNNY!* \n",
        .file_path = "la_content/encounters/molten_bunny.png",
        
        .solutions = {"Let him be", "Give hm head pats", "Take him with you"}
      },
      {
        .name = "Plain of Embers: To Touch Or Not To Touch",
        .conflict = "*It moves without a trace! But it's so tempting to touch it...* \n",
        .file_path = "la_content/encounters/living_dark_flame.png",
        
        .solutions = {"Try to touch it", "Try to blow it out", "Follow it"}
      },
      {
        .name = "Death's Locus: Squirrel Power",
        .conflict = "*This squirrel has learned to wield the corruptive powers!* \n",
        .file_path = "la_content/encounters/dark_squirrel_mage.png",
        
        .solutions = {"Throw an acorn", "Run", "Negotiate peace"}
      },
      {
        .name = "Death's Locus: Squirrel Fanatic",
        .conflict = "*Brainwashed by the corruptive forces, this squirrel isn't natural!* \n",
        .file_path = "la_content/encounters/squirrel_cultist.png",
        
        .solutions = {"Throw an acorn at it", "Scurry away", "Does it like acorns?"}
      },
      {
        .name = "Death's Locus: The Less Fortunate",
        .conflict = "*Was once a happy squirrel, but was unfortunate enough to stumble upon this corruption...* \n",
        .file_path = "la_content/encounters/possessed_squirrel.png",
        
        .solutions = {"Offer an acorn", "Let him be", "Try to comfort him"}
      }
    },

    .encounter_size = 15
  }
};