/*************************************************************************
@file LevelTile.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Declares LevelTile, a plain data class representing a single tile entry
    in a level file. Tiles cover both terrain and obstacles; the type field
    determines how the tile is interpreted by the level loader.

    Members:
       type      - Integer tag identifying the tile category (e.g. obstacle
                   type, platform type). Interpretation is caller-defined.
       pos       - World-space center position.
       half_size - Half the bounding box dimensions, used for AABB collision
                   and rendering.
       velocity  - Movement per second. Zero for static tiles; non-zero for
                   moving platforms or obstacles loaded from file.
       scale     - Uniform size multiplier. Primarily used for obstacles.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#ifndef LEVEL_TILE_HPP
#define LEVEL_TILE_HPP

#include "pch.hpp"

class LevelTile
{
public:
    // Default constructor: initializes all fields to zero.
    LevelTile()
        : type{ 0 }, pos{ 0, 0 }, half_size{ 0, 0 },
        velocity{ 0, 0 }, scale{ 0.0f } {
    }

    // Parameterized constructor: initializes from explicit values.
    // Used when constructing tiles programmatically rather than from file.
    LevelTile(int type, AEVec2 pos, AEVec2 half_size, AEVec2 velocity, f32 scale)
        : type{ type }, pos{ pos }, half_size{ half_size },
        velocity{ velocity }, scale{ scale } {
    }

    LevelTile& operator=(LevelTile const& rhs);

    int    type;
    AEVec2 pos;
    AEVec2 half_size;
    AEVec2 velocity;  // Zero for static tiles.
    f32    scale;     // Primarily used for obstacles.
};

// Deserializes a LevelTile from a whitespace-delimited input stream.
// Field order: type  pos.x pos.y  half_size.x half_size.y  vel.x vel.y  scale
std::istream& operator>>(std::istream& is, LevelTile& tile);

// Serializes a LevelTile to an output stream in the same field order,
// space-delimited and terminated with a newline.
std::ostream& operator<<(std::ostream& os, LevelTile const& tile);

#endif // LEVEL_TILE_HPP