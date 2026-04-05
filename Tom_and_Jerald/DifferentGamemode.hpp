/*************************************************************************
@file    DifferentGamemode.hpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  NIL
@brief
     Declares the Maze gamemode game-state lifecycle functions and
     maze configuration constants.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef GAMESTATE_MAZE_H
#define GAMESTATE_MAZE_H

#include "AEEngine.h"

void GameState_MazeLoad();
void GameState_MazeInit();
void GameState_MazeUpdate();
void GameState_MazeDraw();
void GameState_MazeFree();
void GameState_MazeUnload();

extern int gMazeRewardsEarned;
extern bool gMazeFromPlaying;
extern bool gMazeCompleted;
extern bool gMazeSuccess;

#endif 