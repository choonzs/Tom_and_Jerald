#include "pch.hpp"
#include "LevelTile.hpp"

// Copy Assignment
LevelTile& LevelTile::operator=(LevelTile const& rhs) {
	
	type = rhs.type;
	pos = rhs.pos;
	half_size = rhs.half_size;
	velocity = rhs.velocity;
	scale = rhs.scale;

	return *this;
}

// >> operator overload for instream
std::istream& operator>>(std::istream& is, LevelTile& tile) {
	is >> tile.type
		>> tile.pos.x >> tile.pos.y
		>> tile.half_size.x >> tile.half_size.y
		>> tile.velocity.x >> tile.velocity.y
		>> tile.scale;
	return is;
}

// << operator overload for instream
std::ostream& operator<<(std::ostream& is, LevelTile& tile) {
	is << tile.type << " "
		<< tile.pos.x << " " << tile.pos.y << " "
		<< tile.half_size.x << " " << tile.half_size.y << " "
		<< tile.velocity.x << " " << tile.velocity.y << " "
		<< tile.scale << "\n";
	return is;
}