#include "pch.hpp"

// For testing
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"

namespace {
	s8 font_id;
}

void PauseMenu_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void PauseMenu_Initialize() {

}

void PauseMenu_Update() {
	if (IsMenuKeyTriggered()) {
		clickAudio.Play();
	}

	if (AEInputCheckTriggered(AEVK_1))
	{
		next = GAME_STATE_PLAYING;
	}
	else if (AEInputCheckTriggered(AEVK_2))
	{
		next = GAME_STATE_MENU;
	}
	else {
		//next = GAME_STATE_RESTART;
	}
}

void PauseMenu_Draw() {

}

void PauseMenu_Free() {

}

void PauseMenu_Unload() {
	AEGfxDestroyFont(font_id);
}