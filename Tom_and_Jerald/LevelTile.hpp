#include "pch.hpp"
#ifndef LEVEL_TILE_HPP
#define LEVEL_TILE_HPP
//struct LevelTile { 
//	int type; 
//	AEVec2 pos; 
//	AEVec2 half_size; 
//	AEVec2 velocity; // Only used for moving tiles, can be zero for static tile
//	f32 scale; // Only for obstacles
//};

//TODO finish conversion into class type
class LevelTile {
public: 
	// Ctors & Dtors
	LevelTile() : 
		type{ 0 },
		pos{0,0},
		half_size{0,0},
		velocity{0,0},
		scale{0} {};

	LevelTile(int type, AEVec2 pos, AEVec2 half_size, AEVec2 velocity, f32 scale) :
		type{ type }, pos{ pos }, half_size{ half_size },
		velocity{ velocity }, scale{ scale } {};

	LevelTile& operator=(LevelTile const& rhs);

	// Accesors & Mutator
	int type;
	AEVec2 pos;
	AEVec2 half_size;
	AEVec2 velocity; // Only used for moving tiles, can be zero for static tile
	f32 scale; // Only for obstacles
private:
};
std::istream& operator>>(std::istream& is, LevelTile& tile);
#endif // !LEVEL_TILE_HPP