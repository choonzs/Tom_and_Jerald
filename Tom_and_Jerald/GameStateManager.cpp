/*************************************************************************
@file GameStateManager.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu,
			Loh Kai Xin kaixin.l@digipen.edu,
			Peng Rong Jun Jerald p.rongjunjerald@digipen.edu,
			Ong Jin Ting o.jinting@digipen.edu
@brief
     Implements the Game State Manager (GSM). GSM_Update() maps the current
     game state to its six lifecycle function pointers so the application loop
     can call them without knowing which state is active.

     RESTART and QUIT are sentinel values handled by Main.cpp; they require
     no function pointer assignments here.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#include "pch.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "MainMenu.hpp"
#include "Shop.hpp"
#include "Playing.hpp"
#include "Victory.hpp"
#include "GameOver.hpp"
#include "LevelEditor.hpp"
#include "CustomLevel.hpp"
#include "DifferentGamemode.hpp"
#include "Settings.hpp"
#include "HighScores.hpp"
#include "CreditsScreen.hpp"
#include "Tutorial.hpp"

// ---------------------------------------------------------------------------
// Global state — definitions (declared extern in GameStateManager.hpp)
// ---------------------------------------------------------------------------

int current{};
int previous{};
int next{};
bool gSkipSplash = false;

FP fpLoad = nullptr;
FP fpInitialize = nullptr;
FP fpUpdate = nullptr;
FP fpDraw = nullptr;
FP fpFree = nullptr;
FP fpUnload = nullptr;

// ---------------------------------------------------------------------------
// GSM_Initialize
// ---------------------------------------------------------------------------
// Sets current, previous, and next to startingState so the first call to
// GSM_Update() populates the correct function pointers before any callbacks
// are invoked.
// ---------------------------------------------------------------------------
void GSM_Initialize(int startingState)
{
    current = previous = next = startingState;
}

// ---------------------------------------------------------------------------
// GSM_Update
// ---------------------------------------------------------------------------
// Maps the current game state to its lifecycle function pointers.
// Called by the application loop each frame before invoking fpUpdate/fpDraw,
// and again after a state transition to load the incoming state's pointers.
// ---------------------------------------------------------------------------
void GSM_Update()
{
	switch (current)
	{
	case GAME_STATE_MENU:
		fpLoad = MainMenu_Load;
		fpInitialize = MainMenu_Initialize;
		fpUpdate = MainMenu_Update;
		fpDraw = MainMenu_Draw;
		fpFree = MainMenu_Free;
		fpUnload = MainMenu_Unload;
		break;
	case GAME_STATE_SHOP:
		fpLoad = Shop_Load;
		fpInitialize = Shop_Initialize;
		fpUpdate = Shop_Update;
		fpDraw = Shop_Draw;
		fpFree = Shop_Free;
		fpUnload = Shop_Unload;
		break;
	case GAME_STATE_GAME_OVER:
		fpLoad = GameOver_Load;
		fpInitialize = GameOver_Initialize;
		fpUpdate = GameOver_Update;
		fpDraw = GameOver_Draw;
		fpFree = GameOver_Free;
		fpUnload = GameOver_Unload;
		break;
	case GAME_STATE_PLAYING:
		fpLoad = Playing_Load;
		fpInitialize = Playing_Initialize;
		fpUpdate = Playing_Update;
		fpDraw = Playing_Draw;
		fpFree = Playing_Free;
		fpUnload = Playing_Unload;
		break;
	case GAME_STATE_VICTORY:
		fpLoad = Victory_Load;
		fpInitialize = Victory_Initialize;
		fpUpdate = Victory_Update;
		fpDraw = Victory_Draw;
		fpFree = Victory_Free;
		fpUnload = Victory_Unload;
		break;
	case GAME_STATE_LEVEL_EDITOR: // Added Level Editor routing
		fpLoad = LevelEditor_Load;
		fpInitialize = LevelEditor_Initialize;
		fpUpdate = LevelEditor_Update;
		fpDraw = LevelEditor_Draw;
		fpFree = LevelEditor_Free;
		fpUnload = LevelEditor_Unload;
		break;
	case GAME_STATE_CUSTOM_PLAY:
		fpLoad = CustomLevel_Load;
		fpInitialize = CustomLevel_Initialize;
		fpUpdate = CustomLevel_Update;
		fpDraw = CustomLevel_Draw;
		fpFree = CustomLevel_Free;
		fpUnload = CustomLevel_Unload;
		break;
	case GAME_STATE_SETTINGS:
		fpLoad = Setting_Load;
		fpInitialize = Settings_Initialize;
		fpUpdate = Settings_Update;
		fpDraw = Settings_Draw;
		fpFree = Settings_Free;
		fpUnload = Settings_Unload;
		break;
	case GAME_STATE_MAZE:
		fpLoad = GameState_MazeLoad;
		fpInitialize = GameState_MazeInit;
		fpUpdate = GameState_MazeUpdate;
		fpDraw = GameState_MazeDraw;
		fpFree = GameState_MazeFree;
		fpUnload = GameState_MazeUnload;
		break;
	case GAME_STATE_TUTORIAL:
		fpLoad = Tutorial_Load;
		fpInitialize = Tutorial_Initialize;
		fpUpdate = Tutorial_Update;
		fpDraw = Tutorial_Draw;
		fpFree = Tutorial_Free;
		fpUnload = Tutorial_Unload;
		break;
	case GAME_STATE_HIGHSCORE:
		fpLoad = HighScore_Load;
		fpInitialize = HighScore_Initialize;
		fpUpdate = HighScore_Update;
		fpDraw = HighScore_Draw;
		fpFree = HighScore_Free;
		fpUnload = HighScore_Unload;
		break;
	case GAME_STATE_CREDITS:
		fpLoad = CreditsScreen_Load;
		fpInitialize = CreditsScreen_Initialize;
		fpUpdate = CreditsScreen_Update;
		fpDraw = CreditsScreen_Draw;
		fpFree = CreditsScreen_Free;
		fpUnload = CreditsScreen_Unload;
		break;
	case GAME_STATE_RESTART:
		// RESTART is handled in Main.cpp by resetting current/next
		// to previous. No function pointers needed.
		break;
	case GAME_STATE_QUIT:
		// QUIT exits the main loop. No function pointers needed
		break;
	default:
		break;
	}
}