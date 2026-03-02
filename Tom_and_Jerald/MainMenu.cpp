#include "pch.hpp"

// For testing
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#include "ImgFontInit.hpp"

namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = nullptr;

	int counter{};
	f32 cam_pos_x, cam_pos_y, cam_pos_angle{};
	Camera camera;

	f32 delta_time;
	f32 local_time;

	f32 window_width;
	f32 window_height;

	BOOL mainmenu_flag;
}


void MainMenu_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	ASSETS::Init_Images();
}

void MainMenu_Initialize() {
	createUnitSquare(&unit_square);

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);

	AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

	delta_time = (f32)AEFrameRateControllerGetFrameTime();

	window_width = (AEGfxGetWinMaxX() - AEGfxGetWinMinX());
	window_height = (AEGfxGetWinMaxY() - AEGfxGetWinMinY());

	mainmenu_flag = FALSE; //False on default, triggers main menu after splashscreen
}

void MainMenu_Update() {

	if (local_time < 3.0f) {
		mainmenu_flag = FALSE;

		// Fade out: full opacity at 0s, invisible at 3s
		f32 alpha = 1.0f - (local_time / 3.0f);

		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(alpha);
		AEGfxTextureSet(ASSETS::copyrightLogo, 1.0f, 1.0f);
		drawQuad(unit_square, 0, 0, window_width - 100.0f, window_height - 100.0f, 1.f, 1.f, 1.f, alpha);

		local_time += delta_time; 
		
	}
	else {
		mainmenu_flag = TRUE;
	}


	if (mainmenu_flag == TRUE) {
		if (AEInputCheckTriggered(AEVK_RETURN))
		{
			PlayClick();
			// Moving to playing state
			next = GAME_STATE_PLAYING;
		}
		else if (AEInputCheckTriggered(AEVK_S))
		{
			PlayClick();
			next = GAME_STATE_SHOP;
		}
		else if (AEInputCheckTriggered(AEVK_E))
		{
			PlayClick();
			next = GAME_STATE_LEVEL_EDITOR;
		}
		else if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		{
			PlayClick();
			// Quitting the game
			next = GAME_STATE_QUIT;
		}
		else if (AEInputCheckTriggered(AEVK_C))
		{
			PlayClick();
			next = GAME_STATE_CUSTOM_PLAY;
		}
		else if (AEInputCheckTriggered(AEVK_T))
		{
			PlayClick();
			next = GAME_STATE_SETTINGS;
		}
		else {
			current = GAME_STATE_MENU;
			// Refreshing the next state to stay in menu
			//next = GAME_STATE_RESTART;
		}

		/*for camera movement
		if (AEInputCheckTriggered(AEVK_A)) {
			// Shift left
			camera.Position().x += 10.0f * delta_time;
		}*/
	}
	
}

void MainMenu_Draw() {
	if (mainmenu_flag == TRUE) {

		drawCenteredText(font_id, "TOM AND JERALD", 0.5f, 1.1f,0.0f,0.0f,1.0f,0.0f);

		drawCenteredText(font_id, "START (ENTER)", 0.3f, 0.7f);
		drawCenteredText(font_id, "SHOP (S)", 0.2f, 0.7f);
		drawCenteredText(font_id, "LEVEL EDITOR (E)", 0.1f, 0.7f);
		drawCenteredText(font_id, "EDITOR (L)", 0.0f, 0.7f);
		drawCenteredText(font_id, "PLAY CUSTOM (C)", -0.1f, 0.7f); // <--- ADD THIS LINE
		drawCenteredText(font_id, "Settings (T)", -0.2f, 0.7f);
		drawCenteredText(font_id, "EXIT (ESC)", -0.3f, 0.7f);

		drawCenteredText(font_id, "MOVE: WASD / ARROWS", -0.5f, 0.45f);
		drawCenteredText(font_id, "AVOID THE OBSTACLES FOR 30 SECONDS", -0.6f, 0.45f);
	}
}

void MainMenu_Free() {
	AEGfxMeshFree(unit_square);
}
void MainMenu_Unload() {
	AEGfxDestroyFont(font_id);
	ASSETS::Unload_Images();
}