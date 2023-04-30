
struct sd_player player = { 0 };
struct sd_scurry scurry = { 0 };

struct sd_rewards rewards = { 0 };
struct sd_buff_status buff_status = { 0 };

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
  DB_WAR_ACORNS,
  DB_ENCOUNTER,
  DB_MAIN_CD,
  // skip user id
  DB_PROFICIENCY_LV = 14,
  DB_STRENGTH_LV,
  DB_LUCK_LV,
  // skip user id
  DB_PROFICIENCY_ACORN = 18,
  DB_DEFENSE_ACORN,
  DB_LUCK_ACORN,
  DB_STRENGTH_ACORN,
  DB_ENDURANCE_ACORN,
  // skip user id
  DB_CATNIP = 24
};

enum SCURRY_RES 
{
  DB_SCURRY_OWNER_ID,
  DB_SCURRY_NAME,
  DB_COURAGE,
  DB_WAR_STASH,
  DB_WAR_FLAG
};

// some listed here are identical for my intents and purposes
// but it's nice to have explicit definitions
enum ACTION_TYPE {
  DB_ACTION_SEARCH,
  DB_ACTION_INSERT,
  DB_ACTION_UPDATE,
  DB_ACTION_DELETE
};

enum SCURRY_RANKS 
{
  SEED_NOT,
  ACORN_SNATCHER,
  SEED_SNIFFER,
  OAKFFICIAL,
  ROYAL_NUT
};

enum ITEMS 
{
  ITEM_ACORNS,
  ITEM_ACORN_COUNT,
  ITEM_GOLDEN_ACORN,
  ITEM_WAR_ACORNS,
  ITEM_ENERGY,
  ITEM_HEALTH,
  ITEM_CATNIP,
  ITEM_SIZE
};

struct sd_obj_items items[ITEM_SIZE] = {
  {
    .item = {
      .formal_name = "Acorns",
      .file_path = "items/acorn.png",

      .emoji_name = "acorns",
      .emoji_id = 1045028765392187402
    },

    .stat_ptr = &player.acorns
  },
  {
    .item = {
      .formal_name = "Acorn Count",
      .file_path = "items/acorn_count.png",

      .emoji_name = "acorn_count",
      .emoji_id = 1050407923823677502
    },

    .stat_ptr = &player.acorn_count
  },
  {
    .item = {
      .formal_name = "Golden Acorns",
      .file_path = "items/golden_acorn.png",

      .emoji_name = "golden_acorn",
      .emoji_id = 1045032005420728430
    },

    .stat_ptr = &player.golden_acorns
  },
  {
    .item = {
      .formal_name = "War Acorns",
      .file_path = "scurry_utils/war_acorns.png",

      .emoji_name = "war_acorns",
      .emoji_id = 1066105193655193640
    },

    .stat_ptr = &player.war_acorns
  },
  {
    .item = {
      .formal_name = "Energy",
      .file_path = "items/energy.png",

      .emoji_name = "energy",
      .emoji_id = 911642184397258752
    },

    .stat_ptr = &player.energy
  },
  {
    .item = {
      .formal_name = "HP",
      .file_path = "items/health.png",

      .emoji_name = "health",
      .emoji_id = 1064579080281923655
    },

    .stat_ptr = &player.health
  },
  {
    .item = {
      .formal_name = "Catnip",
      .file_path = "items/catnip.png",

      .emoji_name = "catnip",
      .emoji_id = 1052250368039452732
    },

    .stat_ptr = &player.events.catnip
  }
};

enum ITEM_TYPE {
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

    .stat_ptr = &player.stats.proficiency_lv,
    .price_mult = PROFICIENCY_UNIT,
    .value_mult = PROFICIENCY_VALUE
  },
  {
    .stat = {
      .formal_name = "Luck",
      .file_path = "stats/luck.png",
      .description = "*Multiplies golden acorn "GOLDEN_ACORNS" earnings*",

      .emoji_name = "luck",
      .emoji_id = 1066084537798500372
    },

    .stat_ptr = &player.stats.luck_lv,
    .price_mult = LUCK_UNIT,
    .value_mult = LUCK_VALUE
  },
  {
    .stat = {
      .formal_name = "Strength",
      .file_path = "stats/strength.png",
      .description = "*Adds 50 max "HEALTH" HP*",

      .emoji_name = "strength",
      .emoji_id = 1066084556110831697
    },

    .stat_ptr = &player.stats.strength_lv,
    .price_mult = STRENGTH_UNIT,
    .value_mult = STRENGTH_VALUE
  }
};

enum BUFFS 
{
  BUFF_PROFICIENCY_ACORN,
  BUFF_LUCK_ACORN,
  BUFF_STRENGTH_ACORN,
  BUFF_ENDURANCE_ACORN,
  BUFF_DEFENSE_ACORN,
  BUFFS_SIZE
};

struct sd_obj_items enchanted_acorns[BUFFS_SIZE] = {
  {
    .item = {
      .formal_name = "Acorn of Proficiency",
      .file_path = "enchanted_acorns/proficiency_acorn.png",
      .description = "*Doubles acorn earnings*",

      .emoji_name = "proficiency_acorn",
      .emoji_id = 1045027184475123813
    },

    .stat_ptr = &player.buffs.proficiency_acorn
  },
  {
    .item = {
      .formal_name = "Acorn of Luck",

      .file_path = "enchanted_acorns/luck_acorn.png",
      .description = "*Doubles golden acorn earnings*",

      .emoji_name = "luck_acorn",
      .emoji_id = 1045027183334264872
    },

    .stat_ptr = &player.buffs.luck_acorn
  },
  {
    .item = {
      .formal_name = "Acorn of Strength",

      .file_path = "enchanted_acorns/healing_acorn.png",
      .description = "*Refills some of HP*",

      .emoji_name = "strength_acorn",
      .emoji_id = 1045027180549255208
    },

    .stat_ptr = &player.buffs.strength_acorn
  },
  {
    .item = {
      .formal_name = "Acorn of Endurance",

      .file_path = "enchanted_acorns/endurance_acorn.png",
      .description = "*Refills some of energy*",

      .emoji_name = "endurance_acorn",
      .emoji_id = 1045027181996286022
    },

    .stat_ptr = &player.buffs.endurance_acorn
  },
  {
    .item = {
      .formal_name = "Acorn of Defense",

      .file_path = "enchanted_acorns/defense_acorn.png",
      .description = "*Decreases damage from encounters*",

      .emoji_name = "defense_acorn",
      .emoji_id = 1045027185754390578
    },

    .stat_ptr = &player.buffs.defense_acorn
  }
};

enum SCURRY_ITEMS
{
  SCURRY_ITEM_COURAGE,
  SCURRY_ITEM_WAR_ACORNS,
  SCURRY_ITEM_SIZE
};

struct sd_obj_items scurry_items[SCURRY_ITEM_SIZE] = {
  {
    .item = {
      .formal_name = "Courage",
      .file_path = "scurry_utils/courage.png",

      .emoji_name = "courage",
      .emoji_id = 1045555306832347147
    },

    .stat_ptr = &scurry.courage
  },
  {
    .item = {
      .formal_name = "War Acorns",
      .file_path = "scurry_utils/war_acorns.png",

      .emoji_name = "war_acorns",
      .emoji_id = 1066105193655193640
    },

    .stat_ptr = &scurry.war_acorns
  }
};

enum BUNNY_STORE
{
  BUNNY_ACORNS,
  BUNNY_GOLDEN_ACORNS,
  BUNNY_ENERGY,
  BUNNY_STORE_SIZE
};

enum TYPE_victuals {
  BLUEBERRY_VICTUALS,
  CHERRY_VICTUALS,
  SEED_VICTUALS,
  VICTUALS_SIZE
};

struct sd_victuals victuals[VICTUALS_SIZE] = {
  {
    .item = {
      .formal_name = "Blueberries",
      .file_path = "victuals/blueberry_victuals.png",

      .emoji_name = "blueberry_victuals",
      .emoji_id = 1094282921944305724
    },

    .stat_ptr = &player.acorn_count,
    .item_ref = &items[ITEM_ACORN_COUNT]
  },
  {
    .item = {
      .formal_name = "Cherries",
      .file_path = "victuals/cherry_victuals.png",

      .emoji_name = "cherry_victuals",
      .emoji_id = 1094282923806556251,
    },

    .stat_ptr = &player.health,
    .item_ref = &items[ITEM_HEALTH]
  },
  {
    .item = {
      .formal_name = "Seeds",
      .file_path = "victuals/seed_victuals.png",

      .emoji_name = "seed_victuals",
      .emoji_id = 1094282924750274572
    },

    .stat_ptr = &player.energy,
    .item_ref = &items[ITEM_ENERGY]
  }
};

enum SQUIRREL 
{
  GRAY_SQUIRREL,
  SKELETAL_SQUIRREL,
  SQUIRREL_BOOKIE,
  KING_SQUIRREL,
  SQUIRREL_SIZE
};

struct sd_squirrel squirrels[SQUIRREL_SIZE] = {
  {
    .acorn_count_req = 0, // base squirrel
    
    .squirrel = {
      .formal_name = "Gray Squirrel",
      .file_path = "squirrels/gl_squirrel.png",

      .description = "*Increased chance to not consume energy*",

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

      .description = "*Doubles acorn earning but encounters deal increased damage*",

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

      .description = "*Doubles acorn count but doubles energy cost*",

      .emoji_name = "king_squirrel",
      .emoji_id = 1073298108508803113
    },

    .evo_squirrel  = {
      .formal_name = "King Squirrel Evolve",
      .file_path = "evo_squirrel/king_squirrel_evolve.png",

      .emoji_name = "king_squirrel_evolve",
      .emoji_id = 1073298727017648219
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
  BIOME_SIZE = 4 // is last available biome (temporary)
};

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
        .name = "Desolate Plains: Art of Hunting",
        .conflict = "A camoflauge snare has caught you! It didn't do much, the flimsy thing.",
        .file_path = "gl_content/encounters/bandit_snare.png",

        .solutions = {"Chew through the string", "Run as fast as you can to break the snare", "Wait for another squirrel to help you"}
      },
      {
        .name = "Imperium City: Rat Infestation",
        .conflict = "Rats! They surely don't like you. Who knows what they could be carrying...",
        .file_path = "gl_content/encounters/city_rat.png",

        .solutions = {"Give him a peace offering", "Hug him", "Back off"}
      },
      {
        .name = "Witch Swamp: Is it an Acorn?",
        .conflict = "This acorn looks like an acorn and tastes like an acorn. Is it an acorn?",
        .file_path = "gl_content/encounters/conjured_acorn.png",

        .solutions = {"Investigate the acorn", "Ignore it", "Take the acorn"}
      },
      {
        .name = "Imperium City: Bold Coyotes",
        .conflict = "These coyotes keep inching towards you. They're curious but do you want to take that chance?",
        .file_path = "gl_content/encounters/coyote.png",

        .solutions = {"Take your chances", "Punch him in the snoot and run up a tree", "Shriek to scare him off"}
      },
      {
        .name = "Desolate Plains: Bullseye",
        .conflict = "You spot a bandit hunter in the corner of your eye as he draws his bow!",
        .file_path = "gl_content/encounters/hunter_detection.png",

        .solutions = {"Hide behind a tree", "Lose his sight", "Throw an acorn"}
      },
      {
        .name = "Enchanted Woods: Unique Goldfish",
        .conflict = "This isn't your everyday goldfish. What's he doin' out of the water?",
        .file_path = "gl_content/encounters/magical_goldfish.png",

        .solutions = {"Go around him", "Eat him!", "Push him back into the water"}
      },
      {
        .name = "Desolate Plains: Suspicious Crops",
        .conflict = "Unlike the other crops, this one is sticky...",
        .file_path = "gl_content/encounters/poisoned_crop.png",

        .solutions = {"Walk away", "Try a kernel", "Wipe off the poison in hopes there's no trace"}
      },
      {
        .name = "Imperium City: Rampant Squirrels",
        .conflict = "Still cute and fluffy- sort of. They dont like other squirrels though.",
        .file_path = "gl_content/encounters/savage_squirrel.png",

        .solutions = {"Tame him", "Wait for him to look the other way", "Offer an acorn"}
      },
      {
        .name = "Witch Swamp: Are you you?",
        .conflict = "It's a friendly squirrel! Is it?",
        .file_path = "gl_content/encounters/squirrel_doppleganger.png",

        .solutions = {"Shake his paw", "Indicate you know it's a disguise", "Ignore him"}
      },
      {
        .name = "Witch Swamp: Poison from Below! Above?",
        .conflict = "A witch has spotted you and threw a flask!",
        .file_path = "gl_content/encounters/thrown_flask.png",

        .solutions = {"Dodge the flask", "Try to catch the flask", "Run at the witch!"}
      },
      {
        .name = "Enchanted Woods: Fluffy Unicorn",
        .conflict = "The unicorn seems friendly... perhaps you could just sneak that acorn from under him?",
        .file_path = "gl_content/encounters/unicorn.png",

        .solutions = {"Cause a distraction", "Bolt out and hope you scare it", "Aim for the throat!"}
      },
      {
        .name = "Enchanted Woods: Burly Pupper",
        .conflict = "A werewolf has spotted you and begins to chase you!",
        .file_path = "gl_content/encounters/werewolf.png",

        .solutions = {"Run for your life!", "Aim for the throat!", "Run into a nearby bush"}
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
        .name = "Forgotten City: Unhappy Camels",
        .conflict = "This camel doesn't look too happy. Watch out for the spit!",
        .file_path = "sp_content/encounters/angry_camel.png",

        .solutions = {"Sneak around", "Aim for the face!", "Cause a distraction"}
      },
      {
        .name = "Sandy Shores: Desparate Bobcat",
        .conflict = "There's an acorn next to the feasting bobcat.",
        .file_path = "sp_content/encounters/bobcat.png",

        .solutions = {"Don't look!", "Snatch it and hope you don't get eaten", "Shake a nearby bush"}
      },
      {
        .name = "Sandy Shores: Sleeping Cobra",
        .conflict = "You've caught the attention of this cobra!",
        .file_path = "sp_content/encounters/cobra.png",

        .solutions = {"Flee the scene", "Aim for the neck!", "Dodge the lunge"}
      },
      {
        .name = "Forgotten City: Starving Coyotes",
        .conflict = "You've accidentally approached a pack of coyotes!",
        .file_path = "sp_content/encounters/coyote.png",

        .solutions = {"Snatch the acorn and run", "Look somewhere else", "Run to the nearby group of people"}
      },
      {
        .name = "Wormhole: Shifting Doom",
        .conflict = "A massive worm is shifting through the sands...",
        .file_path = "sp_content/encounters/death_worm.png",

        .solutions = {"Lure and fight it", "Move in the opposite direction", "Run past it"}
      },
      {
        .name = "Golden Dunes: Desert Scavengers",
        .conflict = "A pack of hyenas are giving you the death stare...",
        .file_path = "sp_content/encounters/hyena.png",

        .solutions = {"Try to touch one", "Aim for the throat!", "Run to a nearby carcass"}
      },
      {
        .name = "Golden Dunes: The Curious Jackal",
        .conflict = "This jackal doesn't seem threatened nor scared. He's, curious?",
        .file_path = "sp_content/encounters/jackal.png",

        .solutions = {"Hand him an acorn", "Give him head pats", "Ignore him and scurry away"}
      },
      {
        .name = "Wormhole: Blood-sucking Fiends!",
        .conflict = "Leeches! Leeches everywhere! Small, but best not let too many latch on!",
        .file_path = "sp_content/encounters/leech.png",

        .solutions = {"Crush them", "Scurry away!", "Run to a nearby carcass"}
      },
      {
        .name = "Sandy Shores: Distracting Vibrations",
        .conflict = "The rattling of the rattlesnake's tail catches your attention and you lock eyes...",
        .file_path = "sp_content/encounters/rattlesnake.png",

        .solutions = {"Ignore it and scurry away", "Passively approach it", "Aim for the throat!"}
      },
      {
        .name = "Forgotten City: Vicious Land Lobsters",
        .conflict = "A scorpion finds you and charges to attack you!",
        .file_path = "sp_content/encounters/scorpion.png",

        .solutions = {"Dodge the pincer", "Throw an acorn", "Scurry Away!"},
      },
      {
        .name = "Ancient Wastelands: Here Comes the Calvary!",
        .conflict = "A living skeleton? A horse? Better hope it's not that unicorn...",
        .file_path = "sp_content/encounters/skeletal_horse.png",

        .solutions = {"Cause a distraction", "Bolt out and hope you scare it", "Aim for the throat!"}
      },
      {
        .name = "Ancient Wastelands: Squirrel Meets Squirrel",
        .conflict = "What looks to be a skeletal squirrel makes eye contaact with you... Well? You can't just ignore it!",
        .file_path = "sp_content/encounters/skeletal_squirrel.png",

        .solutions = {"Shake its paw", "Tame it", "Offer an acorn"}
      },
      {
        .name = "Golden Dunes: Desert Puppers",
        .conflict = "They look a bit hungry. Maybe not a friendly foe...",
        .file_path = "sp_content/encounters/wild_dog.png",

        .solutions = {"Offer an acorn", "Show them to some food", "Scurry awway!"}
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
        .name = "Breached Woods: Collapsed Caves",
        .conflict = "This bear doesn't seem too happy his home was destroyed...",
        .file_path = "ne_content/encounters/angry_bear.png",

        .solutions = {"Offer an acorn", "Boop his snoot", "Give him a hug"}
      },
      {
        .name = "Light's Refuge: The Kind Beast",
        .conflict = "This bear doesn't seem one bit intmidated! Perhaps he's friendly?",
        .file_path = "ne_content/encounters/bear.png",

        .solutions = {"Give him head pats", "Offer an acorn", "Don't take your chances"}
      },
      {
        .name = "Ruins of Luxos: Sewage Rats",
        .conflict = "[dry heaves] Those rats STINK! Where have they been, the sewers?",
        .file_path = "ne_content/encounters/city_rat.png",

        .solutions = {"Flee their stench", "Hold your breath", "Throw them in water"}
      },
      {
        .name = "Breached Woods: Consumed Elk",
        .conflict = "Taken by the growing corruption, his mind isn't his own!",
        .file_path = "ne_content/encounters/consumed_elk.png",

        .solutions = {"Aim for the throat!", "Squeal!", "Flee into a tree"}
      },
      {
        .name = "Breached Woods: Corrupt Squirrel",
        .conflict = "This one doesn't want to shake your paw... You're his snack!",
        .file_path = "ne_content/encounters/corrupt_squirrel.png",

        .solutions = {"Outrun him", "Force his paw out", "Offer an acorn"}
      },
      {
        .name = "Nature's Dying Light: Desparate Elk",
        .conflict = "This elk doesn't appear to be aggressive. Perhaps he's just hungry?",
        .file_path = "ne_content/encounters/elk.png",

        .solutions = {"Offer an acorn", "Lead him to a food source", "Give him head pats"}
      },
      {
        .name = "Light's Refuge: Curious Fox",
        .conflict = "The fox looks at you and sits expectantly.",
        .file_path = "ne_content/encounters/fox.png",

        .solutions = {"Say \"roll over\" in squirrel", "Give him head pats", "Throw an acorn"}
      },
      {
        .name = "Nature's Dying Light: Threatened Moose",
        .conflict = "A defensive moose detects you and charges at you!",
        .file_path = "ne_content/encounters/moose.png",

        .solutions = {"Run under him", "Climb up a tree", "Aim for the throat!"}
      },
      {
        .name = "Ruins of Luxos: Aggressive Trash Pandas",
        .conflict = "Is it growling? Is it smiling? Does it want your acorns?",
        .file_path = "ne_content/encounters/raccoon.png",

        .solutions = {"Bonk his head with an acorn", "Throw an acorn and run", "Climb up a building"}
      },
      {
        .name = "Light's Refuge: Santa's Good Boi",
        .conflict = "Looks like Santa may have lost one! He seems nice...",
        .file_path = "ne_content/encounters/reindeer.png",

        .solutions = {"Give him a random carrot", "Give him chin rubs", "Take him with you"}
      },
      {
        .name = "Ruins of Luxos: Ancient Squirrel",
        .conflict = "This squirrel has seen a bit... Can he see?",
        .file_path = "ne_content/encounters/skeletal_squirrel.png",

        .solutions = {"Hope he can see your acorn offer", "Shake his paw if he can feel it", "Tame him if he know who you are"}
      },
      {
        .name = "Nature's Dying Light: Spooked Wolf",
        .conflict = "A twig you stepped on startled the wolf! He's approaching...",
        .file_path = "ne_content/encounters/wolf.png",

        .solutions = {"Run up a tree", "Hide in a bush", "Aim for throat!"}
      },
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
        .conflict = "An arctic fox's head pops above the snow and locks eyes with you.",
        .file_path = "dg_content/encounters/arctic_fox.png",

        .solutions = {"Stare harder", "Wave", "Keep running"}
      },
      {
        .name = "Boreal Forest: Sneaky Puppers",
        .conflict = "You hear snow patting and realize a wolf is after you!",
        .file_path = "dg_content/encounters/arctic_wolf.png",

        .solutions = {"Throw an acorn", "Scurry faster", "Aim for the throat!"}
      },
      {
        .name = "Death's Valley: Less Fluffy Bunn- uh, Fox",
        .conflict = "This fox has been at the mercy of some necromancers... Not friendly.",
        .file_path = "dg_content/encounters/consumed_fox.png",

        .solutions = {"Offer an acorn for its troubles", "Scurry away", "Dance to lighten its mood"}
      },
      {
        .name = "Death's Valley: Nature's Dilemma",
        .conflict = "So cute but scary at the same!",
        .file_path = "dg_content/encounters/consumed_hare.png",

        .solutions = {"Stare in confusion", "Attempt to pat its head", "Don't look in its eyes!"}
      },
      {
        .name = "Death's Valley: Angry Teddy Bear",
        .conflict = "This polar bear doesn't look too happy about his situation...",
        .file_path = "dg_content/encounters/consumed_polar_bear.png",

        .solutions = {"Appease with an acorn", "Scurry away", "Pretend like he isn't there"}
      },
      {
        .name = "Necrotic Lakes: Purple Energy",
        .conflict = "That- that's a squirrel? So dark. So smokey. So... deadly...",
        .file_path = "dg_content/encounters/dark_skeletal_squirrel.png",

        .solutions = {"Squeal!", "Ignore it", "Run up a tree"}
      },
      {
        .name = "Kingdom of Ice: Desparate for Warmth",
        .conflict = "This squirrel has been wandering aimlessly for hours. But he finds a warm companion...",
        .file_path = "dg_content/encounters/frost_squirrel.png",

        .solutions = {"Scurry away", "Don't make eye contact", "Cough"}
      },
      {
        .name = "Necrotic Lakes: Squirrel-o-mancer",
        .conflict = "Does he raise- tiny fluffballs of doom? Should you find out?",
        .file_path = "dg_content/encounters/necro-squirrel.png",

        .solutions = {"Approach", "Bonk him on the cabeza with an acorn", "Don't want to know"}
      },
      {
        .name = "Boreal Forest: Waddle Waddle",
        .conflict = "This curious penguin approaches you with intrigument.",
        .file_path = "dg_content/encounters/penguin.png",

        .solutions = {"Stop him before he pecks", "Offer an acorn", "Scurry on"}
      },
      {
        .name = "Kingdom of Ice: Nice Teddy Bear",
        .conflict = "This polar bear appears to be a bit lonely...",
        .file_path = "dg_content/encounters/polar_bear.png",

        .solutions = {"Snuggle with him", "Offer an acorn", "Let him be"}
      },
      {
        .name = "Lost City of Honor: Bony Teddy Bear",
        .conflict = "Those claws look sharp and it doesn't look happy...",
        .file_path = "dg_content/encounters/skeletal_bear.png",

        .solutions = {"Scurry away", "Aim for the throat!", "Ride it to where you want to go next"}
      },
      {
        .name = "Lost City of Honor: Future Me?",
        .conflict = "We bare uncanny resemblance, but it prefers to be alone.",
        .file_path = "dg_content/encounters/skeletal_squirrel.png",

        .solutions = {"Tame it", "Leash it", "Poke with an acorn until it does something"}
      },
      {
        .name = "Lost City of Honor: Not so Fluffy...",
        .conflict = "Can it see? Can it hear? What do you do?",
        .file_path = "dg_content/encounters/skeletal_wolf.png",

        .solutions = {"Squeal to test its hearing", "Throw an acorn to see if it can feel", "Boop his skeletal snoot"}
      },
      {
        .name = "Kingdom of Ice: Definitely a Fluffy Bunny",
        .conflict = "He's so cute, small ans squishy you could just-",
        .file_path = "dg_content/encounters/snowshoe_hare.png",

        .solutions = {"Bring him home", "Give him head pats", "Call him Squishy"}
      },
      {
        .name = "Necrotic Lakes: Re-foraging",
        .conflict = "This poor guy was in the unfortunate hands of a necro-squirrel.",
        .file_path = "dg_content/encounters/undead_squirrel.png",

        .solutions = {"Offer an acorn", "Put him out of his misery", "Hug him"}
      },
    },

    .encounter_size = 15
  }
};