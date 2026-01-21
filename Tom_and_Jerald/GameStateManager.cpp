#include "pch.h"

#include "GameStateList.h"
#include "GameStateManager.h"
#include "MainMenu.h"
#include "GameOver.h"
#include "Playing.h"
#include "Victory.h"
#include "Shop.hpp"

int current{}, previous{}, next{};

FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;
void GSM_Initialize(int startingState)
{
	current = previous = next = startingState;
}

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
	case GAME_STATE_RESTART:
		break;
	case GAME_STATE_QUIT:
		break;
	default:
		break;
	}
}