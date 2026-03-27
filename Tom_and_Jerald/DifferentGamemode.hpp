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