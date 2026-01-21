#include "pch.h"

// For testing
#include "GameStateList.h"
#include "Utils.h"
#include "GameStateManager.h"

namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = NULL;
}

void GameOver_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void GameOver_Initialize() {
	createUnitSquare(&unit_square);
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetCamPosition(0.0f, 0.0f);
}

void GameOver_Update() {
	if (AEInputCheckTriggered(AEVK_1))
	{
		next = GAME_STATE_PLAYING;
	}
	else if (AEInputCheckTriggered(AEVK_2))
	{
		next = GAME_STATE_MENU;
	}
	else if (AEInputCheckTriggered(AEVK_S))
	{
		next = GAME_STATE_SHOP;
	}
	else if (AEInputCheckTriggered(AEVK_3))
	{
		next = GAME_STATE_QUIT;
	}
	else {
		//next = GAME_STATE_RESTART;
	}
}

void GameOver_Draw() {
	drawCenteredText(font_id, "GAME OVER", 0.35f, 1.0f);
	drawCenteredText(font_id, "TRY AGAIN (1)", 0.05f, 0.7f);
	drawCenteredText(font_id, "RETURN TO MAIN MENU (2)", -0.1f, 0.7f);
	drawCenteredText(font_id, "SHOP (S)", -0.2f, 0.7f);
	drawCenteredText(font_id, "EXIT (3)", -0.3f, 0.7f);
}

void GameOver_Free() {
	AEGfxMeshFree(unit_square);
}

void GameOver_Unload() {
	AEGfxDestroyFont(font_id);
}