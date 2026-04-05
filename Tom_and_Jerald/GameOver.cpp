/*************************************************************************
@file    GameOver.cpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@brief
     Implements the Game Over state: final score display, high-score
     name entry via keyboard, and navigation back to menu, shop, or
     to retry the current stage.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#include "pch.hpp"

// For testing
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"
#include "Credits.hpp"
#include "HighScores.hpp"
#include "ImgFontInit.hpp"

namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = NULL;

	// Score system
	Leaderboard* currentboard = nullptr;
	std::string scoreText("SCORE : ");
	bool new_score{ false };

	std::string newName{};
	//
}

void GameOver_Load() {
	ASSETS::Init_Font();
	font_id = ASSETS::Font();
}

void GameOver_Initialize() {
	createUnitSquare(&unit_square);
	AEGfxSetCamPosition(0.0f, 0.0f);

	scoreText = "SCORE : " + std::to_string(credits_this_round);

	currentboard = new Leaderboard("Assets/data/HighScores.txt");
	if (credits_this_round > currentboard->LowestScore()) {
		new_score = true; // NEW SCORE
	}
	
}

void GameOver_Update() {
	if (IsMenuKeyTriggered()) clickAudio.Play();

	if (new_score) {
		// type in name here
		// using ascii table 
		for (char c{ 'A' }; c < 'Z'; c++) {
			if (AEInputCheckTriggered(c)) {
				if (newName.size() < 10) newName += c;
			}
		}
		// backspace
		if (AEInputCheckCurr(AEVK_BACK)) {
			if (!newName.empty())
				newName.pop_back();
		}

		// enter
		if (AEInputCheckTriggered(AEVK_RETURN)) {
			//Edge case empty player name
			if (newName.empty()) {
				newName = "rat";
			}
			// save to leaderboard
			Leaderboard::Score newScore{ newName, credits_this_round };
			currentboard->AddScore(newScore); // you need this function
			currentboard->UpdateLeaderboard("Assets/data/HighScores.txt");
			new_score = false; // done typing
			credits_this_round = 0; // reset score for next game
		}
	}
	else {
		if (AEInputCheckTriggered(AEVK_1))
		{
			next = GAME_STATE_PLAYING;
		}
		else if (AEInputCheckTriggered(AEVK_2))
		{
			gSkipSplash = true;
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
}

void GameOver_Draw() {
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	
	if (new_score) {
		drawCenteredText(font_id, "NEW HIGHSCORE!", 0.35f, 1.0f);
		drawCenteredText(font_id, "ENTER NAME :", 0.15f, 1.0f);
		drawCenteredText(font_id, newName.c_str(), -.1f, 1.0f);
	}
	else {
		drawCenteredText(font_id, "GAME OVER", 0.35f, 1.0f);
		drawCenteredText(font_id, scoreText.c_str(), 0.15f, 1.0f);
		drawCenteredText(font_id, "TRY AGAIN (1)", 0.05f, 0.7f);
		drawCenteredText(font_id, "RETURN TO MAIN MENU (2)", -0.1f, 0.7f);
		drawCenteredText(font_id, "SHOP (S)", -0.2f, 0.7f);
		drawCenteredText(font_id, "EXIT (3)", -0.3f, 0.7f);
	}
}

void GameOver_Free() {
	if (unit_square) { AEGfxMeshFree(unit_square); unit_square = nullptr; }
	// dealloc memory for current board
	delete currentboard;
	currentboard = nullptr;
}

void GameOver_Unload() {
    ASSETS::Unload_Font();
}