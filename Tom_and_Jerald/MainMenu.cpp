#include "pch.h"

// For testing
#include "GameStateList.h"
#include "Utils.h"
#include "GameStateManager.h"

namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = NULL;

	int counter{};
}


void MainMenu_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void MainMenu_Initialize() {
	createUnitSquare(&unit_square);

	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetCamPosition(0.0f, 0.0f);
}

void MainMenu_Update() {
	if (AEInputCheckTriggered(AEVK_RETURN))
	{
		// Moving to playing state
		next = GAME_STATE_PLAYING;
	}
	else {
		// Refreshing the next state to stay in menu
		next = GAME_STATE_RESTART;
	}

}

void MainMenu_Draw() {
	drawCenteredText(font_id, "TOM AND JERALD", 0.4f, 1.1f);
	drawCenteredText(font_id, "START (ENTER)", 0.1f, 0.7f);
	drawCenteredText(font_id, "EXIT (ESC)", -0.05f, 0.7f);
	drawCenteredText(font_id, "MOVE: WASD / ARROWS", -0.25f, 0.45f);
	drawCenteredText(font_id, "AVOID THE OBSTACLES FOR 30 SECONDS", -0.35f, 0.45f);
}

void MainMenu_Free() {
	AEGfxMeshFree(unit_square);
}
void MainMenu_Unload() {
	AEGfxDestroyFont(font_id);
}