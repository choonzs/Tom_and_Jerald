/*************************************************************************
@file    LevelEditor.hpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@brief
     Declares the Level Editor game-state lifecycle functions and tile
     type definitions.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#pragma once
#include <vector>
#include <string>

// Tile types
enum TileType {
    TILE_EMPTY = 0,
    TILE_SQUARE = 1,
    TILE_SPIKE = 2
};

void LevelEditor_Load();
void LevelEditor_Initialize();
void LevelEditor_Update();
void LevelEditor_Draw();
void LevelEditor_Free();
void LevelEditor_Unload();