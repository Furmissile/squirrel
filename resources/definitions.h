enum SLICE_TYPES 
{
  TYPE_EMPTY,
  TYPE_ACORN_COUNT,
  TYPE_ACORNS,
  TYPE_GOLDEN_ACORNS,
  TYPE_CONJURED_ACORNS,
  TYPE_SPRING_SEEDS,
  TYPE_SUMMER_CHERRIES,
  TYPE_FALL_BLUEBERRIES,
  TYPE_WINTER_CATNIP,
  TYPE_WAR_ACORNS,
  TYPE_RIBBONED_ACORN,
  ITEM_SIZE
};

struct sd_slice_types slice_types[ITEM_SIZE] = {
  {
    .item = {
      .emoji_name = "empty_acorn",
      .emoji_id = 1191889906696474665
    },

    .emoji_literal = EMPTY_ACORN
  },
  {
    .item = {
      .formal_name = "Acorn Count",
      .file_path = "items/acorn_count.png",

      .emoji_name = "acorn_count",
      .emoji_id = 1164894602491351113
    },

    .emoji_literal = ACORN_COUNT
  },
  {
    .item = {
      .formal_name = "Acorns",
      .file_path = "items/acorn.png",

      .emoji_name = "acorn",
      .emoji_id = 1164894600926863360
    },

    .emoji_literal = ACORNS,
    .base_value = 10
  },
  {
    .item = {
      .formal_name = "Golden Acorns",
      .file_path = "items/golden_acorn.png",

      .emoji_name = "golden_acorn",
      .emoji_id = 1164894605788074066
    },

    .emoji_literal = GOLDEN_ACORNS,
    .base_value = 25
  },
  {
    .item = {
      .formal_name = "Conjured Acorn",
      .file_path = "items/conjured_acorn.png",

      .emoji_name = "conjured_acorn",
      .emoji_id = 1164897240771461151
    },

    .emoji_literal = CONJURED_ACORNS
    // x2 pie value
  },
  {
    .item = {
      .formal_name = "Seeds",
      .file_path = "victuals/seed_victuals.png",

      .emoji_name = "seed_victuals",
      .emoji_id = 1164899715456975131
    },

    .emoji_literal = SEED_VICTUALS,
    .base_value = 50
  },
  {
    .item = {
      .formal_name = "Cherries",
      .file_path = "victuals/cherry_victuals.png",

      .emoji_name = "cherry_victuals",
      .emoji_id = 1164899714467115089
    },

    .emoji_literal = CHERRY_VICTUALS,
    .base_value = 75
  },
  {
    .item = {
      .formal_name = "Blueberries",
      .file_path = "victuals/blueberry_victuals.png",

      .emoji_name = "blueberry_victuals",
      .emoji_id = 1164899712751636531
    },

    .emoji_literal = BLUEBERRY_VICTUALS,
    .base_value = 100
  },
  {
    .item = {
      .formal_name = "Catnip",
      .file_path = "items/catnip.png",

      .emoji_name = "catnip",
      .emoji_id = 1164897238749818961
    },

    .emoji_literal = CATNIP,
    .base_value = 150
  },
  {
    .item = {
      .formal_name = "War Acorns",
      .file_path = "items/war_acorns.png",

      .emoji_name = "war_acorns",
      .emoji_id = 1164897241698418688
    },

    .emoji_literal = WAR_ACORNS
  },
  {
    .item = {
      .formal_name = "Ribboned Acorn",
      .file_path = "items/ribboned_acorn.png",

      .emoji_name = "ribboned_acorn",
      .emoji_id = 1180504068750311515
    },

    .emoji_literal = RIBBONED_ACORN,
    .base_value = 250
  }
};

enum PIECES 
{
  PIECE_CORNER_ONE,
  PIECE_CENTER,
  PIECE_FIRST_TWO,
  PIECE_LAST_TWO,
  PIECE_ROW,
  PIECE_CORNER_THREE,
  PIECE_L,
  // PIECE_L_INV,

  PIECES_SIZE
};

struct sd_pie pieces[PIECES_SIZE] = 
{
  /*
    x . .
    .   .
    . . .
  */
  {
    .decoded_buf = { 1, 0, 0, 0, 0, 0, 0, 0 }
  },

  /*
    . x .
    .   .
    . . .
  */
  {
    .decoded_buf = { 0, 1, 0, 0, 0, 0, 0, 0 }
  },

  /*
    . . .
    x   .
    x . .
  */
  {
    .decoded_buf = { 0, 0, 0, 0, 0, 0, 1, 1 }
  },

  /*
    x . .
    x   .
    . . .
  */
  {
    .decoded_buf = { 1, 0, 0, 0, 0, 0, 0, 1 }
  },

  /*
    x . .
    x   .
    x . .
  */
  {
    .decoded_buf = { 1, 0, 0, 0, 0, 0, 1, 1 }
  },

  /*
    x x .
    x   .
    . . .
  */
  {
    .decoded_buf = { 1, 1, 0, 0, 0, 0, 0, 1 }
  },

  /*
    x . .
    x   .
    x x .
  */
  {
    .decoded_buf = { 1, 0, 0, 0, 0, 1, 1, 1 }
  }

  /*
    x x .
    x   .
    x . .
  */
  // {
  //   .decoded_buf = { 1, 1, 0, 0, 0, 0, 1, 1 }
  // }
};

enum MISC 
{
  MISC_ADD,
  MISC_SCURRY_ICON,
  MISC_CROWN,
  MISC_WELCOME,
  MISC_BOXES,
  MISC_ALERT,
  MISC_HELP,
  MISC_TREASURE,
  MISC_SIZE
};

struct sd_file_data misc[MISC_SIZE] = {
  {
    .formal_name = "plus",
    .file_path = "misc/plus.png",

    .emoji_name = "plus",
    .emoji_id = 1177068547973849130
  },
  {
    .formal_name = "scurry",
    .file_path = "misc/guild_icon.png",

    .emoji_name = "guild_icon",
    .emoji_id = 1020714354351542362
  },
  {
    .formal_name = "crown",
    .file_path = "misc/leader.png",

    .emoji_name = "leader",
    .emoji_id = 1035976066965196861
  },
  {
    .formal_name = "Welcome!",
    .file_path = "misc/git_welcome.gif"
  },
  {
    .emoji_name = "boxes",
    .emoji_id = 1191447753662922802
  },
  {
    .file_path = "misc/no_acorns.png",

    .emoji_name = "quest_marker",
    .emoji_id = 1164898155150708736
  },
  {
    .file_path = "misc/no_acorns.png",

    .emoji_name = "no_acorns",
    .emoji_id = 1164898153884029012
  },
  {
    .file_path = "misc/lost_stash.png",

    .emoji_name = "lost_stash",
    .emoji_id = 1164898152906764349
  }
};

enum SQUIRRELS
{
  GRAY_SQUIRREL,
  SKELETAL_SQUIRREL,
  SQUIRREL_BOOKIE,
  ANGELIC_SQUIRREL,
  KING_SQUIRREL,
  FLORAL_SQUIRREL,
  GOLDEN_SQUIRREL,
  BLACK_SQUIRREL,
  ACRON_SQUIRREL,
  FLOATY_SQUIRREL,
  BEACH_SQUIRREL,
  PIRATE_SQUIRREL,
  VEGGIE_SQUIRREL,
  RED_SQUIRREL,
  ALCHEMIST_SQUIRREL,
  DEMONIC_SQUIRREL,
  WIZARD_SQUIRREL,
  FROST_SQUIRREL,
  HOARDER_SQUIRREL,
  SANTA_SQUIRREL,
  INFURNO_SQUIRREL,
  SQUIRREL_SIZE
};

struct sd_squirrel squirrels[SQUIRREL_SIZE] = {
  // BASE SQUIRREL
  {
    .squirrel = {
      .formal_name = "Gray Squirrel",
      .file_path = "squirrels/gray_squirrel.png",

      .description = "*Your traditional squirrel. Vicious and ferocious.*",

      .emoji_name = "gray_squirrel",
      .emoji_id = 1014655900373504130
    }
  },
  // TRADITIONAL SQUIRRELS
  {
    .cost = FIRST_SQUIRREL,
    .squirrel = {
      .formal_name = "Skeletal Squirrel",
      .file_path = "squirrels/skeletal_squirrel.png",

      .description = "*Seen one too many moons...*",

      .emoji_name = "skeletal_squirrel",
      .emoji_id = 1072150369599234058
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .squirrel = {
      .formal_name = "Squirrel Bookie",
      .file_path = "squirrels/squirrel_bookie.png",

      .description = "*He got the numbers if you got the coin.*",

      .emoji_name = "squirrel_bookie",
      .emoji_id = 1050175322286469250
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .squirrel = {
      .formal_name = "Angelic Squirrel",
      .file_path = "squirrels/angelic_squirrel.png",

      .description = "*100% pure holiness... and maybe a whisper of greed.*",

      .emoji_name = "angelic_squirrel",
      .emoji_id = 1140690614137139314
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .squirrel = {
      .formal_name = "King Squirrel",
      .file_path = "squirrels/king_squirrel.png",

      .description = "*Your royal fuzziness. Stands proud amongst his peers.*",

      .emoji_name = "king_squirrel",
      .emoji_id = 1073298108508803113
    }
  },
  // SPRING
  {
    .cost = FIRST_SQUIRREL,
    .squirrel = {
      .formal_name = "Floral Squirrel",
      .file_path = "squirrels/spring_squirrels/floral_squirrel.png",

      .description = "*Stuck his nose in one too many flowers.*",

      .emoji_name = "floral_squirrel",
      .emoji_id = 1167134584425697401
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .squirrel = {
      .formal_name = "Golden Squirrel",
      .file_path = "squirrels/spring_squirrels/golden_squirrel.png",

      .description = "*Did he eat a golden acorn?*",

      .emoji_name = "golden_squirrel",
      .emoji_id = 1167134585306501130
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .squirrel = {
      .formal_name = "Black Squirrel",
      .file_path = "squirrels/spring_squirrels/black_squirrel.png",

      .description = "*Something about him is unsettling...*",

      .emoji_name = "black_squirrel",
      .emoji_id = 1167134583414853733
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .squirrel = {
      .formal_name = "Acron",
      .file_path = "squirrels/spring_squirrels/acron.png",

      .description = "*Always searching for the next golden acorn.*",

      .emoji_name = "acron",
      .emoji_id = 1167134581686808597
    }
  },

  // SUMMER
  {
    .cost = FIRST_SQUIRREL,
    .squirrel = {
      .formal_name = "Floaty Squirrel",
      .file_path = "squirrels/summer_squirrels/floaty_squirrel.png",

      .description = "*Soaking up the sun.*",

      .emoji_name = "floaty_squirrel",
      .emoji_id = 1167134612468809899
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .squirrel = {
      .formal_name = "Beach Squirrel",
      .file_path = "squirrels/summer_squirrels/beach_squirrel.png",

      .description = "*What happened at the beach?*",

      .emoji_name = "beach_squirrel",
      .emoji_id = 1167134608727486485
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .squirrel = {
      .formal_name = "Squirrel Pirate",
      .file_path = "squirrels/summer_squirrels/squirrel_pirate.png",

      .description = "*Proper party crasher.*",

      .emoji_name = "squirrel_pirate",
      .emoji_id = 1167134613697740880
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .squirrel = {
      .formal_name = "Veggie Snatcher",
      .file_path = "squirrels/summer_squirrels/veggie_snatcher.png",

      .description = "*Raided more gardens than you can count!*",

      .emoji_name = "veggie_snatcher",
      .emoji_id = 1167134610572980244
    }
  },

  // FALL
  {
    .cost = FIRST_SQUIRREL,
    .squirrel = {
      .formal_name = "Red Squirrel",
      .file_path = "squirrels/fall_squirrels/red_squirrel.png",

      .description = "*Blends with the Fall leaves.*",

      .emoji_name = "red_squirrel",
      .emoji_id = 1167134556965585006
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .squirrel = {
      .formal_name = "Squirrel Alchemist",
      .file_path = "squirrels/fall_squirrels/squirrel_alchemist.png",

      .description = "*Ready to create the next enchanted acorn.*",

      .emoji_name = "squirrel_alchemist",
      .emoji_id = 1167134558190321744
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .squirrel = {
      .formal_name = "Demonic Squirrel",
      .file_path = "squirrels/fall_squirrels/demonic_squirrel.png",

      .description = "*Fresh from the depths of Hell.*",

      .emoji_name = "demonic_squirrel",
      .emoji_id = 1167134555338178560
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .squirrel = {
      .formal_name = "Archwizard Flooftail",
      .file_path = "squirrels/fall_squirrels/archwizard_flooftail.png",

      .description = "*Sourcerer with powers of an unknown origin.*",

      .emoji_name = "archwizard_flooftail",
      .emoji_id = 1167134553098440794
    }
  },

  // WINTER
  {
    .cost = FIRST_SQUIRREL,
    .squirrel = {
      .formal_name = "Frost Squirrel",
      .file_path = "squirrels/winter_squirrels/frost_squirrel.png",

      .description = "*Loves the snow a little too much.*",

      .emoji_name = "frost_squirrel",
      .emoji_id = 1167134638712557579
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .squirrel = {
      .formal_name = "Hoarder Squirrel",
      .file_path = "squirrels/winter_squirrels/hoarder_squirrel.png",

      .description = "*No one will take his acorns...*",

      .emoji_name = "hoarder_squirrel",
      .emoji_id = 1167134639748546570
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .squirrel = {
      .formal_name = "Santa Squirrel",
      .file_path = "squirrels/winter_squirrels/santa_squirrel.png",

      .description = "*The inventor of acorn.*",

      .emoji_name = "santa_squirrel",
      .emoji_id = 1167134642466455612
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .squirrel = {
      .formal_name = "Infurno",
      .file_path = "squirrels/winter_squirrels/infurno.png",

      .description = "*Fluffy jellybe- spawn of Hell.*",

      .emoji_name = "infurno",
      .emoji_id = 1167134641577279548
    }
  },
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

/* Encounter solutions are ordered by: action, logic, and chance */
struct sd_biome biomes[BIOME_SIZE] = {
  { // GRASSLANDS
    .biome_scene_path = "gl_content/grasslands_scene.gif",

    .biome_icon = {
      .formal_name = "Grasslands",
      .file_path = "gl_content/gl_icon.png",

      .emoji_name = "gl_icon",
      .emoji_id = 1164900062091018240
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

            .context = "Inhabited by all sorts of mythical creatures."
          },
          {
            .name = "Burly Pupper",
            .conflict = "*A werewolf has spotted you and begins to chase you!* \n",
            .file_path = "gl_content/encounters/werewolf.png",

            .context = "Trees spiral in all sorts of directions, radiating ancient magic."
          },
          {
            .name = "Unique Goldfish",
            .conflict = "*This isn't your everyday goldfish. What's he doin' out of the water?* \n",
            .file_path = "gl_content/encounters/magical_goldfish.png",

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

            .context = "The city is made entirely of white marble."
          },
          {
            .name = "Bold Coyotes",
            .conflict = "*These coyotes keep inching towards you. They're curious but do you want to take that chance?* \n",
            .file_path = "gl_content/encounters/coyote.png",

            .context = "Its citizens are known for their curiosity, intellect, and profound fascination with the nearby Enchanted Woods."
          },
          {
            .name = "Rampant Squirrels",
            .conflict = "*Still cute and fluffy- sort of. He doesn't like other squirrels though.* \n",
            .file_path = "gl_content/encounters/savage_squirrel.png",

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

            .context = "Once farmland that fed the city's residents."
          },
          {
            .name = "Bullseye",
            .conflict = "*You spot a bandit hunter in the corner of your eye as he draws his bow!* \n",
            .file_path = "gl_content/encounters/hunter_detection.png",

            .context = "Now swarms with rebels who are unhappy with the Imperium City's conditions."
          },
          {
            .name = "Suspicious Crops",
            .conflict = "*Unlike the other crops, this one is sticky...* \n",
            .file_path = "gl_content/encounters/poisoned_crop.png",

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

            .context = "Witches were once reputable alchemists in the city."
          },
          {
            .name = "Are you you?",
            .conflict = "*It's a friendly squirrel! Is it?* \n",
            .file_path = "gl_content/encounters/squirrel_doppleganger.png",

            .context = "Twisted by the growing corruption, the witches started conducting dark rituals on animals."
          },
          {
            .name = "Poison from Below! Above?",
            .conflict = "*A witch has spotted you and threw a flask!* \n",
            .file_path = "gl_content/encounters/thrown_flask.png",

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
      .emoji_id = 1164900129850019860
    },

    .intro = "Welcome to the "SEEPING_SANDS_ICON" **Seeping Sands**! \n"
        "The biome is a vast desert wasteland with little to no vegetation that sees frequent sandstorms. "
        "The main settlement here is the Forgotten City whose agriculture malpractice caused the fertile soil to erode.",

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

            .context = "The desert is also devoid of prey causing predators to become desperate."
          },
          {
            .name = "Sleeping Cobra",
            .conflict = "*You've caught the attention of this cobra!* \n",
            .file_path = "sp_content/encounters/cobra.png",

            .context = "These areas see a dense population of snakes."
          },
          {
            .name = "Distracting Vibrations",
            .conflict = "*The rattling of the rattlesnake's tail catches your attention and you lock eyes...* \n",
            .file_path = "sp_content/encounters/rattlesnake.png",

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

            .context = "The city is built entirely of sandstone and partially overridden with sand."
          },
          {
            .name = "Unhappy Camels",
            .conflict = "*This camel doesn't look too happy. Watch out for the spit!* \n",
            .file_path = "sp_content/encounters/angry_camel.png",

            .context = "A once prosperous city for trade went to ruin due to growing internal economic problems."
          },
          {
            .name = "Starving Coyotes",
            .conflict = "*You've accidentally approached a pack of coyotes!* \n",
            .file_path = "sp_content/encounters/coyote.png",

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

            .context = "The vast golden dunes spanning the majority of the desert."
          },
          {
            .name = "The Curious Jackal",
            .conflict = "*This jackal doesn't seem threatened nor scared. He's, curious?* \n",
            .file_path = "sp_content/encounters/jackal.png",

            .context = "Animals who don't come in contact with people are often curious, but not all."
          },
          {
            .name = "Desert Puppers",
            .conflict = "*They look a bit hungry. Maybe not a friendly foe...* \n",
            .file_path = "sp_content/encounters/wild_dog.png",

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

            .context = "These wastelands are where many ancient battles took place."
          },
          {
            .name = "Squirrel Meets Squirrel",
            .conflict = "*What looks to be a skeletal squirrel locks eyes with you... Well? You can't just ignore it!* \n",
            .file_path = "sp_content/encounters/skeletal_squirrel.png",

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

            .context = "Massive worms shift in the sands and feel the vibrations of footsteps."
          },
          {
            .name = "Blood-sucking Fiends!",
            .conflict = "*Leeches! Leeches everywhere! Small, but best not to let too many latch on!* \n",
            .file_path = "sp_content/encounters/leech.png",

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
      .emoji_id = 1164900105615327243
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
            .conflict = "*This bear doesn't seem one bit intimidated! Perhaps he's friendly?* \n",
            .file_path = "ne_content/encounters/bear.png",

            .context = "A local oracle resides here to heal the forest."
          },
          {
            .name = "Curious Fox",
            .conflict = "*The fox looks at you and sits expectantly.* \n",
            .file_path = "ne_content/encounters/fox.png",

            .context = "The animals that roam here are very friendly."
          },
          {
            .name = "Santa's Good Boi",
            .conflict = "*Looks like Santa may have lost one! He seems nice...* \n",
            .file_path = "ne_content/encounters/reindeer.png",

            .context = "Although this section of pine forest is protected, the oracle is struggling to maintain the light."
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

            .context = "The border between the protected and corrupt pine forests."
          },
          {
            .name = "Threatened Moose",
            .conflict = "*A defensive moose detects you and charges at you!* \n",
            .file_path = "ne_content/encounters/moose.png",

            .context = "The presence of the corruption causes these animals to be on edge and untrusting."
          },
          {
            .name = "Spooked Wolf",
            .conflict = "*A twig you stepped on startled a wolf! He's approaching...* \n",
            .file_path = "ne_content/encounters/wolf.png",

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

            .context = "The effects of the corruption have compromised many of the natural structures."
          },
          {
            .name = "Consumed Elk",
            .conflict = "*Taken by the growing corruption, his mind isn't his own!* \n",
            .file_path = "ne_content/encounters/consumed_elk.png",

            .context = "Animals will be left with purple marks when they eat corrupt matter."
          },
          {
            .name = "Corrupt Squirrel",
            .conflict = "*This one doesn't want to shake your paw... You're his snack!* \n",
            .file_path = "ne_content/encounters/corrupt_squirrel.png",

            .context = "Once the animals have been consumed by the corruption, its mind is no longer its own."
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

            .context = "Once a colony of Imperium City, it fell to the corruption roughly 63 years ago."
          },
          {
            .name = "Aggressive Trash Pandas",
            .conflict = "*Is it growling? Is it smiling? Does it want your acorns?* \n",
            .file_path = "ne_content/encounters/raccoon.png",

            .context = "While this area remains somewhat intact, many animals reside here to buy themselves time to escape the corruption."
          },
          {
            .name = "Ancient Squirrel",
            .conflict = "*This squirrel has seen a bit... Can he see?* \n",
            .file_path = "ne_content/encounters/skeletal_squirrel.png",

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
      .emoji_id = 1164900043036299335
    },

    .intro = "Welcome to "DEATH_GRIP_ICON" **Death's Grip**! \n"
        "This biome is a desolate tundra with little life. It gets its name from its naturally unforgiving climate and the growing threat of corruption. What was once a white and bright tundra is now riddled with necrotic energy and heavy fog. \n"
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

            .context = "The average height of snow for this area is roughly 3 feet."
          },
          {
            .name = "Sneaky Puppers",
            .conflict = "*You hear snow patting and realize a wolf is stalking you!* \n",
            .file_path = "dg_content/encounters/arctic_wolf.png",

            .context = "The animals are hostile from hunger, no protection from the elements, and the neighboring corruption."
          },
          {
            .name = "Waddle Waddle",
            .conflict = "*This curious penguin approaches you with intrigument.* \n",
            .file_path = "dg_content/encounters/penguin.png",

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

            .context = "These lakes are dense with black mist."
          },
          {
            .name = "Squirrel-o-mancer",
            .conflict = "*Does he raise- tiny fluffballs of doom? Should you find out?* \n",
            .file_path = "dg_content/encounters/necro-squirrel.png",

            .context = "As you draw closer to the corruption, these corrupt animals develop strategy and critical thinking."
          },
          {
            .name = "Re-foraging",
            .conflict = "*This poor guy was in the unfortunate hands of a necro-squirrel.* \n",
            .file_path = "dg_content/encounters/undead_squirrel.png",

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
            .name = "Desperate for Warmth",
            .conflict = "*This squirrel has been wandering aimlessly for hours. But he finds a warm companion...* \n",
            .file_path = "dg_content/encounters/frost_squirrel.png",

            .context = "Built with packed snow and hardened ice supported by boreal beams."
          },
          {
            .name = "Nice Teddy Bear",
            .conflict = "*This polar bear appears to be a bit lonely...* \n",
            .file_path = "dg_content/encounters/polar_bear.png",

            .context = "The animals that manage to survive here are far and few between."
          },
          {
            .name = "Definitely a Fluffy Bunny",
            .conflict = "*He's so cute, small and squishy you could just-* \n",
            .file_path = "dg_content/encounters/snowshoe_hare.png",

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

            .context = "Closest to the corruption, this city was the first to fall."
          },
          {
            .name = "Future Me?",
            .conflict = "*We bare uncanny resemblance, but it prefers to be alone.* \n",
            .file_path = "dg_content/encounters/skeletal_squirrel.png",

            .context = "Some animals here must have been raised by necromancers and their behavior tells us they retain their memories."
          },
          {
            .name = "Not so Fluffy...",
            .conflict = "*Can it see? Can it hear? What do you do?* \n",
            .file_path = "dg_content/encounters/skeletal_wolf.png",

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

            .context = "Animals here are used by necromancers to do their bidding."
          },
          {
            .name = "Nature's Dilemma",
            .conflict = "*So cute but scary at the same!* \n",
            .file_path = "dg_content/encounters/consumed_hare.png",

            .context = "Even the cutest of animals fall to the corruption."
          },
          {
            .name = "Angry Teddy Bear",
            .conflict = "*This polar bear doesn't look too happy about his situation...* \n",
            .file_path = "dg_content/encounters/consumed_polar_bear.png",

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
      .emoji_id = 1164900082315968542
    },

    .intro = "Welcome to the "LAST_ACORN_ICON" **The Last Acorn**! \n"
        "This biome looks identical to the Grasslands, but it's characterized by dense purple fog, humming dark energy, the blood of the sacrificed coursing through the dirt, and crazed squirrels who once inhabited the area. Its name came from the squirrel colonies because squirrels are lured in by conjured acorns and it's the last acorn they ever see. \n"
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

            .context = "These animals are born and raised by the corruption."
          },
          {
            .name = "Fox Zoomies",
            .conflict = "*The foxes notices you, locks eyes with you, pupils grow large...* \n",
            .file_path = "la_content/encounters/fox_familiar.png",
            
            .context = "The corruption trains these familiars to be unsuspectingly deadly predators."
          },
          {
            .name = "The Less Fortunate",
            .conflict = "*Was once a happy squirrel, but was unfortunate enough to stumble upon this corruption...* \n",
            .file_path = "la_content/encounters/possessed_squirrel.png",
            
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
            
            .context = "The only spot the corruption didn't take for unknown reasons."
          },
          {
            .name = "Happy Elk",
            .conflict = "*This elk is happy. He's surrounded by food and other elk!* \n",
            .file_path = "la_content/encounters/elk.png",
            
            .context = "Animals here are happier than those in the oracle's protection."
          },
          {
            .name = "Snoozing Bear",
            .conflict = "*This bear seems content in his slumber. But there's an acorn behind him!* \n",
            .file_path = "la_content/encounters/bear.png",
            
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
            
            .context = "Illusions are created by the witches to trap unsuspecting animals."
          },
          {
            .name = "Random Goldfish",
            .conflict = "*This goldfish is flopping around on the ground far away from any body of water...* \n",
            .file_path = "la_content/encounters/conjured_goldfish.png",
            
            .context = "Witches here and can recreate your memories as if they were reality."
          },
          {
            .name = "It's a Normal Squirrel, Right?",
            .conflict = "*A suspicious-looking squirrel is feasting on some victuals.* \n",
            .file_path = "la_content/encounters/squirrel_doppleganger.png",
            
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
            
            .context = "These creatures are neither animal nor familiar, but hellspawn."
          },
          {
            .name = "A fellow Bunny?",
            .conflict = "*But he's just A BUNNY!* \n",
            .file_path = "la_content/encounters/molten_bunny.png",
            
            .context = "The grass is hot to the touch and releases sparks."
          },
          {
            .name = "To Touch Or Not To Touch",
            .conflict = "*It moves without a trace! But it's so tempting to touch it...* \n",
            .file_path = "la_content/encounters/living_dark_flame.png",
            
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
            
            .context = "These squirrels draw off the corruption's power with the intent of dominating the world."
          },
          {
            .name = "Squirrel Fanatic",
            .conflict = "*Brainwashed by the corruptive forces, this squirrel isn't natural!* \n",
            .file_path = "la_content/encounters/squirrel_cultist.png",
            
            .context = "Any human or animal that enters the corruption never makes it out in one piece."
          },
          {
            .name = "The Talking Squirrel",
            .conflict = "*It speaks but its mouth doesn't move? What is it saying?* \n",
            .file_path = "la_content/encounters/familiar_skeletal_squirrel.png",
            
            .context = "Mind control can become so powerful that some animals can chant. But what are they saying?"
          }
        },

        .section_size = 3
      }
    },

    .biome_size = 5
  }
};