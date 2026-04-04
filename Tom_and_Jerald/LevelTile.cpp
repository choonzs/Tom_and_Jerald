/*************************************************************************
@file LevelTile.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Implements LevelTile copy assignment and stream operators for
    serialization/deserialization of level files.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "LevelTile.hpp"

// ---------------------------------------------------------------------------
// LevelTile::operator=
// ---------------------------------------------------------------------------
// Copy assignment: performs a memberwise copy of all tile fields.
// ---------------------------------------------------------------------------
LevelTile& LevelTile::operator=(LevelTile const& rhs)
{
    if (this != &rhs)
    {
        type = rhs.type;
        pos = rhs.pos;
        half_size = rhs.half_size;
        velocity = rhs.velocity;
        scale = rhs.scale;
    }
    return *this;
}

// ---------------------------------------------------------------------------
// operator>>
// ---------------------------------------------------------------------------
// Deserializes a LevelTile from a whitespace-delimited input stream.
// Field order must match the serialized format written by operator<<:
//   type  pos.x pos.y  half_size.x half_size.y  vel.x vel.y  scale
// ---------------------------------------------------------------------------
std::istream& operator>>(std::istream& is, LevelTile& tile)
{
    is >> tile.type
        >> tile.pos.x >> tile.pos.y
        >> tile.half_size.x >> tile.half_size.y
        >> tile.velocity.x >> tile.velocity.y
        >> tile.scale;
    return is;
}

// ---------------------------------------------------------------------------
// operator<<
// ---------------------------------------------------------------------------
// Serializes a LevelTile to an output stream, space-delimited and
// terminated with a newline. Field order mirrors operator>>.
// ---------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, LevelTile const& tile)
{
    os << tile.type << " "
        << tile.pos.x << " " << tile.pos.y << " "
        << tile.half_size.x << " " << tile.half_size.y << " "
        << tile.velocity.x << " " << tile.velocity.y << " "
        << tile.scale << "\n";
    return os;
}