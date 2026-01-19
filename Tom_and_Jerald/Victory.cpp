#include "pch.h"

// For testing
#include "GameStateList.h"
#include "Utils.h"
#include "GameStateManager.h"

namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = nullptr;
}

void Victory_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void Victory_Initialize() {
	createUnitSquare(&unit_square);
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(nullptr, 0.0f, 0.0f);
	AEGfxSetCamPosition(0.0f, 0.0f);
}

void Victory_Update() {
	if (AEInputCheckTriggered(AEVK_1))
	{
		next = GAME_STATE_PLAYING;
	}
	else if (AEInputCheckTriggered(AEVK_2))
	{
		next = GAME_STATE_MENU;
	}
	else if (AEInputCheckTriggered(AEVK_3))
	{
		next = GAME_STATE_QUIT;
	}
	else {
		next = GAME_STATE_RESTART;
	}
}

void Victory_Draw() {
	drawCenteredText(font_id, "CONGRATULATIONS!", 0.35f, 1.0f);
	drawCenteredText(font_id, "PROCEED TO NEXT STAGE (N)", 0.05f, 0.7f);
	drawCenteredText(font_id, "SAVE AND RETURN TO MAIN MENU (M)", -0.1f, 0.7f);
	drawCenteredText(font_id, "EXIT (ESC)", -0.25f, 0.7f);
}

void Victory_Free() {
	AEGfxMeshFree(unit_square);
}

void Victory_Unload() {
	AEGfxDestroyFont(font_id);
}