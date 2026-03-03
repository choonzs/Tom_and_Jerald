#include "pch.hpp"
#include "LevelTile.hpp"

std::istream& operator>>(std::istream& is, LevelTile& tile) {
	is >> tile.type
		>> tile.pos.x >> tile.pos.y
		>> tile.half_size.x >> tile.half_size.y
		>> tile.velocity.x >> tile.velocity.y
		>> tile.scale;
	return is;
}