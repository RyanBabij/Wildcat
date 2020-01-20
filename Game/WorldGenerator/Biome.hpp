#pragma once
#ifndef WILDCAT_MISC_WORLDGENERATOR_BIOME_HPP
#define WILDCAT_MISC_WORLDGENERATOR_BIOME_HPP

/* Wildcat: Biome.hpp
#include <WorldGenerator/Biome.hpp>

These are hardcoded globals for now.

These really ought to be turned into objects at some point, so that biomes can be customised and whatnot.

*/

enum enumBiome { NOTHING=0, OCEAN=1, GRASSLAND=2, FOREST=3, DESERT=4, MOUNTAIN=5, SNOW=6, HILLY=7, JUNGLE=8, WETLAND=9, STEPPES=10, CAVE=11, RUIN=12, ICE=13, RIVER=14, UNDERGROUND=15};
const std::string biomeName [16] = { "nothing", "ocean", "grassland", "forest", "desert", "mountain", "snow", "hilly", "jungle", "wetland", "steppes", "cave", "ruin", "ice", "river", "underground" };
const int N_BIOMES = 15;

const unsigned char biomeRed [16] = { 254, 50, 66, 0, 200, 80, 220, 80, 0, 20, 180, 0, 255, 255, 100, 100 };
const unsigned char biomeGreen [16] = { 254, 50, 180, 120, 200, 80, 220, 140, 70, 150, 120, 0, 0, 255, 100, 100 };
const unsigned char biomeBlue [16] = { 254, 240, 66, 0, 20, 80, 240, 80, 0, 200, 40, 0, 255, 255, 240, 100 };

#endif
