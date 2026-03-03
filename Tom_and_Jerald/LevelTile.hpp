#include "pch.hpp"
#ifndef LEVEL_TILE_HPP
#define LEVEL_TILE_HPP
struct LevelTile { 
	int type; 
	AEVec2 pos; 
	AEVec2 half_size; 
	AEVec2 velocity; // Only used for moving tiles, can be zero for static tile
	f32 scale; // Only for obstacles
};
std::istream& operator>>(std::istream& is, LevelTile& tile);
#endif // !LEVEL_TILE_HPP