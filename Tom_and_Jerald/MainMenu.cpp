/*************************************************************************
@file    MainMenu.cpp
@Author  Loh Kai Xin kaixin.l@digipen.edu
@Co-authors  Tan Choon Ming choonming.tan@digipen.edu
@brief
     Implements the main menu: DigiPen/team splash screens, navigation
     buttons, custom-level select panel, and tutorial launch prompt.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#include "ImgFontInit.hpp"
#include "Animation.hpp"
#include "UI.hpp"

namespace {
	//Assets
	s8 font_id;										//Reference to font ID created in ImgFontInit, for drawing text
	AEGfxVertexList* unit_square = nullptr;			//For Splashscreen drawing
	AEGfxVertexList* gameLogo = nullptr;			//For Game logo
	AEGfxVertexList* background = nullptr;			//For background

	//Ceamera
	Camera camera;
	//Parallax using mouse
	f32 parallax_x = 0.0f;
	f32 parallax_y = 0.0f;

	//Time/frame
	f32 delta_time;									
	f32 local_time;

	//World coords
	f32 window_width;				
	f32 window_height;		

	//Mouse coords									//For tracking for hovering on UI and parallax
	s32 mouseX_int{};
	s32 mouseY_int{};

	//Flags
	BOOL mainMenu_flag;								//For main menu access, plays after teamname splashscreen
	BOOL teamName_flag;								//For teamname splashscreen, plays after copyright splashscreen
	BOOL quitting_flag;								//Used to trigger quitting the game destructive action when player clicks escape or closes window
	BOOL tutorialPromptOpen;				        //Used for tutorial prompt selection

	//Values for UI buttons
	const f32 btnSize = 200.0f;						//For button size
	const f32 colSpacing = 325.0f;					//For spacing between buttons
	const f32 rowTop = 50.0f;						//Top row coords
	const f32 rowBot = -200.0f;						//Bottom row coords

	//Select Custom Level
	BOOL levelSelectOpen{ false };
	//Txt files start w 1
	unsigned int select_level{ 1 };
	std::vector<std::string> level_files;
	int levelSelectCursor = 0;						//Which item is highlighted
	int levelScrollOffset = 0;						//How much the list is scrolled
	const int k_visible_rows = 5;					//Visible rows in the level select menu

}

//Forward declarations
static void ScanLevelFiles(std::vector<std::string>& level_files);

//LOAD
void MainMenu_Load() {
	ASSETS::Init_Images();
	ASSETS::Init_Font();
	font_id = ASSETS::Font();
}

//INITIALIZE
void MainMenu_Initialize() {
	//Initialize assets
	createUnitSquare(&unit_square);
	createUnitSquare(&gameLogo, 0.25f, 0.25f);
	createUnitSquare(&background, 0.25f, 0.25f);

	//Initialize blending modes
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);

	//Initialize camera position
	AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

	//Initialize delta time
	delta_time = (f32)AEFrameRateControllerGetFrameTime();

	//Initialize window size
	window_width = (AEGfxGetWinMaxX() - AEGfxGetWinMinX());
	window_height = (AEGfxGetWinMaxY() - AEGfxGetWinMinY());

	//Initialize flags
	if (gSkipSplash) {
		local_time = 6.0f;
		mainMenu_flag = TRUE;
		teamName_flag = FALSE;
		gSkipSplash = false;
	}
	else {
		mainMenu_flag = FALSE;						//False on default, triggers main menu after teamname
		teamName_flag = FALSE;						//False on defualt, triggers after splashscreen
	}

	quitting_flag = FALSE;							//False on default, triggers when player clicks escape or closes window
	tutorialPromptOpen = FALSE;						//False on default, triggers when player clicks on play


	//Initialize animation
	ANIMATION::gameLogo.ImportFromFile("Assets/AnimationData.txt");			//Total rows + columns file
	ANIMATION::gameLogo.Clip_Select(0, 0, 4, 5.0f);							//Row, start col, frames, fps for game logo

	ANIMATION::background.ImportFromFile("Assets/AnimationData.txt");
	ANIMATION::background.Clip_Select(0, 0, 4, 2.0f);						//For background

	//Initialize buttons for UI
	//Top row: start, shop, lvlEditor, lvlSelector
	UI::startBtn.UI_Init(-colSpacing * 1.5f, rowTop, btnSize, btnSize);		//For game start button
	UI::startBtn.UI_Select(UI::UIButtons::buttonKey::start);
	UI::shopBtn.UI_Init(-colSpacing * 0.5f, rowTop, btnSize, btnSize);		//For shop button
	UI::shopBtn.UI_Select(UI::UIButtons::buttonKey::shop);
	UI::lvlEditBtn.UI_Init(colSpacing * 0.5f, rowTop, btnSize, btnSize);	//For level editor button
	UI::lvlEditBtn.UI_Select(UI::UIButtons::buttonKey::lvlEditor);
	UI::lvlSelectorBtn.UI_Init(colSpacing * 1.5f, rowTop, btnSize, btnSize);//For level selector button
	UI::lvlSelectorBtn.UI_Select(UI::UIButtons::buttonKey::lvlSelector);

	//Bottom row: highscore, settings, credits, exit
	UI::highscoreBtn.UI_Init(-colSpacing * 1.5f, rowBot, btnSize, btnSize); //For highscore button
	UI::highscoreBtn.UI_Select(UI::UIButtons::buttonKey::highscore);
	UI::settingsBtn.UI_Init(-colSpacing * 0.5f, rowBot, btnSize, btnSize);  //For settings button
	UI::settingsBtn.UI_Select(UI::UIButtons::buttonKey::settings);
	UI::creditsBtn.UI_Init(colSpacing * 0.5f, rowBot, btnSize, btnSize);	//For credits button
	UI::creditsBtn.UI_Select(UI::UIButtons::buttonKey::credits);
	UI::exitBtn.UI_Init(colSpacing * 1.5f, rowBot, btnSize, btnSize);		//For exit button
	UI::exitBtn.UI_Select(UI::UIButtons::buttonKey::exit);

	//Yes and No buttons for tutorial prompt
	UI::yesBtn.UI_Init(-150.0f, -70.0f, btnSize	, btnSize);					//For yes button
	UI::yesBtn.UI_Select(UI::UIButtons::buttonKey::yes);
	UI::noBtn.UI_Init(150.0f, -70.0f, btnSize, btnSize);					//For no button
	UI::noBtn.UI_Select(UI::UIButtons::buttonKey::no);

	//Initialize audio
	backgroundAudio.Play();

	//Show cursor
	AEInputShowCursor(true);
}

//UPDATE
void MainMenu_Update() {
	//Update camera
	camera.Update();

	//Update time
	delta_time = (f32)AEFrameRateControllerGetFrameTime();

	//Update animation according to time
	ANIMATION::gameLogo.Anim_Update(delta_time);
	ANIMATION::background.Anim_Update(delta_time);

	//Update cursor positon
	AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

	//Camera parallax
	f32 nx = (static_cast<f32>(mouseX_int) / window_width) * 2.0f - 1.0f;
	f32 ny = (static_cast<f32>(mouseY_int) / window_height) * 2.0f - 1.0f;

	ny = -ny;

	f32 target_x = nx * 30.0f;
	f32 target_y = ny * 30.0f;

	parallax_x += (target_x - parallax_x) * 0.1f;
	parallax_y += (target_y - parallax_y) * 0.1f;

	//Play audio on click
	if (AEInputCheckTriggered(AEVK_LBUTTON)) {
		clickAudio.Play();
	}

	//Update spashscreen to fade out
	if (local_time < 3.0f) {
		//Set teamname to not show first
		teamName_flag = FALSE;

		//Fade out, full opacity at 0s, invisible at 3s
		f32 splashScreenAlpha = 1.0f - (local_time / 3.0f);

		//Default settings - MUST KEEP, prevents game crash
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		//Runs the splashscreen
		AEGfxSetTransparency(splashScreenAlpha);

		//Draw the background and splashscreen
		AEGfxTextureSet(NULL, 1.0f, 1.0f);
		drawQuad(background, 0, 0.0, window_width, window_height, 1.f, 1.f, 1.f, 1.f);
		AEGfxTextureSet(ASSETS::copyrightLogo, 1.0f, 1.0f);
		drawQuad(unit_square, 0, 0, window_width - 100.0f, window_height - 100.0f, 1.f, 1.f, 1.f, splashScreenAlpha);

		//Constantly update time to create effect
		local_time += delta_time;
	}
	else {
		//Plays teamname screen after
		teamName_flag = TRUE;
	}

	//Destructive confirmation for quitting the game
	if (quitting_flag == TRUE) {
		//Press Y to quit, N to cancel
		if (AEInputCheckTriggered(AEVK_Y)) {
			//Quitting the game
			next = GAME_STATE_QUIT;
		}
		else if (AEInputCheckTriggered(AEVK_N)) {
			quitting_flag = FALSE;
		}
	}

	//After splashscreen and logo, run main menu
	if (mainMenu_flag == TRUE) {
		//Open the prompt for tutorial
		if (tutorialPromptOpen){
			if (AEInputCheckTriggered(AEVK_ESCAPE)){
				//Escape tutorial
				tutorialPromptOpen = FALSE;
				return;
			}


			if (AEInputCheckTriggered(AEVK_LBUTTON)){

				AEInputGetCursorPosition(&mouseX_int, &mouseY_int);
				f32 mouseX = static_cast<f32>(mouseX_int) - (window_width * 0.5f);
				f32 mouseY = -(static_cast<f32>(mouseY_int) - (window_height * 0.5f));

				if (UI::yesBtn.UI_IsHovered(mouseX, mouseY)) {
					tutorialPromptOpen = FALSE;
					next = GAME_STATE_TUTORIAL;
				}
				else if (UI::noBtn.UI_IsHovered(mouseX, mouseY)) {
					tutorialPromptOpen = FALSE;
					next = GAME_STATE_PLAYING;
				}
			}

			return;
		}
		// Mouse click on buttons
		if (AEInputCheckTriggered(AEVK_LBUTTON)) {
			
			AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

			f32 mouseX = static_cast<f32>(mouseX_int) - (window_width * 0.5f);
			f32 mouseY = -(static_cast<f32>(mouseY_int) - (window_height * 0.5f));

			if (UI::startBtn.UI_IsHovered(mouseX, mouseY))      tutorialPromptOpen = TRUE;
			if (UI::shopBtn.UI_IsHovered(mouseX, mouseY))       next = GAME_STATE_SHOP;
			if (UI::lvlEditBtn.UI_IsHovered(mouseX, mouseY))    next = GAME_STATE_LEVEL_EDITOR;
			if (UI::lvlSelectorBtn.UI_IsHovered(mouseX, mouseY)) { ScanLevelFiles(level_files); levelSelectOpen = true; }
			if (UI::highscoreBtn.UI_IsHovered(mouseX, mouseY))  next = GAME_STATE_HIGHSCORE;
			if (UI::settingsBtn.UI_IsHovered(mouseX, mouseY))   next = GAME_STATE_SETTINGS;
			if (UI::creditsBtn.UI_IsHovered(mouseX, mouseY))    next = GAME_STATE_CREDITS;
			if (UI::exitBtn.UI_IsHovered(mouseX, mouseY))       quitting_flag = TRUE;


		}
		//Temp for switching levels
		if (AEInputCheckTriggered(AEVK_A)) {
			if ((select_level) > 1) { --select_level; }
		}
		else if (AEInputCheckTriggered(AEVK_D)) {
			++select_level;
		}

		
		if (levelSelectOpen) {
			if (AEInputCheckTriggered(AEVK_UP) || AEInputCheckTriggered(AEVK_W)) {
				if (levelSelectCursor > 0) --levelSelectCursor;
				if (levelSelectCursor < levelScrollOffset)
					levelScrollOffset = levelSelectCursor;
			}
			if (AEInputCheckTriggered(AEVK_DOWN) || AEInputCheckTriggered(AEVK_S)) {
				if (levelSelectCursor < (int)level_files.size())++levelSelectCursor;
				if (levelSelectCursor >= levelScrollOffset + k_visible_rows)
					levelScrollOffset = levelSelectCursor - k_visible_rows + 1;
			}
			if (AEInputCheckTriggered(AEVK_RETURN)) {
				select_level = levelSelectCursor + 1;
				namespace fs = std::filesystem;
				fs::create_directory("MapLevel");
				std::ofstream outFile("MapLevel/LoadLevel.txt", std::ios::trunc);
				if (outFile.is_open()) { outFile << select_level; outFile.close(); }
				next = GAME_STATE_CUSTOM_PLAY;
				levelSelectOpen = false;
			}
			if (AEInputCheckTriggered(AEVK_ESCAPE)) {
				levelSelectOpen = false;
			}
			// Mouse click selection
			if (AEInputCheckTriggered(AEVK_LBUTTON)) {
				s32 mx{}, my{};
				AEInputGetCursorPosition(&mx, &my);
				f32 mouseX = static_cast<f32>(mx) - (window_width * 0.5f);
				f32 mouseY = -(static_cast<f32>(my) - (window_height * 0.5f));

				f32 list_top = 150.0f;
				f32 row_height = 35.0f;
				f32 panel_half_w = 200.0f;

				for (int i = 0; i < (int)level_files.size(); ++i) {
					f32 row_y = list_top - i * row_height;
					if (mouseX >= -panel_half_w && mouseX <= panel_half_w &&
						mouseY >= row_y - row_height * 0.5f && mouseY <= row_y + row_height * 0.5f)
					{
						int actual_index = i + levelScrollOffset;
						levelSelectCursor = actual_index;

						namespace fs = std::filesystem;
						fs::create_directory("MapLevel");

						std::ofstream outFile("MapLevel/LoadLevel.txt", std::ios::trunc);
						if (outFile.is_open()) {
							outFile << (actual_index + 1);
							outFile.close();
						}

						next = GAME_STATE_CUSTOM_PLAY;
						levelSelectOpen = false;
						break;
					}
				}
			}

			return; // block all other input while open
		}


		// Menu Controls
		if (AEInputCheckTriggered(AEVK_RETURN))
		{
			tutorialPromptOpen = TRUE;
		}
		else if (AEInputCheckTriggered(AEVK_S))
		{
			next = GAME_STATE_SHOP;
		}
		else if (AEInputCheckTriggered(AEVK_E))
		{
			next = GAME_STATE_LEVEL_EDITOR;
		}
		else if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		{
			quitting_flag = true;

		}
		else if (AEInputCheckTriggered(AEVK_I))
		{
			ScanLevelFiles(level_files);
			levelSelectOpen = !levelSelectOpen;
			levelSelectCursor = 0;
			levelScrollOffset = 0;
		}
		else if (AEInputCheckTriggered(AEVK_T))
		{
			next = GAME_STATE_SETTINGS;
		}
		else if (AEInputCheckTriggered(AEVK_M))
		{
			next = GAME_STATE_MAZE;
		}
		else if (AEInputCheckTriggered(AEVK_H))
		{
			next = GAME_STATE_HIGHSCORE;
		}
		else if (AEInputCheckTriggered(AEVK_C))
		{
			next = GAME_STATE_CREDITS;
		}
		
		else {
				//DO nothing 
		}
	}

	if (teamName_flag == TRUE) {
		if (local_time < 6.0f) {
			mainMenu_flag = FALSE;

			AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);

			f32 teamNameAlpha{};

			// Fade IN: 3.0 -> 4.0s
			if (local_time < 4.0f) {
				teamNameAlpha = (local_time - 3.0f) / 1.0f;
			}
			// Hold: 4.0 -> 4.5s
			else if (local_time < 4.5f) {
				teamNameAlpha = 1.0f;
			}
			// Fade OUT: 4.5 -> 6.0s
			else {
				teamNameAlpha = 1.0f - ((local_time - 4.5f) / 1.5f);
			}

			teamNameAlpha = AEClamp(teamNameAlpha, 0.0f, 1.0f);

			// Only draw if actually visible
			if (teamNameAlpha > 0.01f) {
				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
				AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
				AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);
				AEGfxSetTransparency(teamNameAlpha);

				drawCenteredText(font_id, "TOM AND", 0.05f, 2.0f, 0.0f, 0.0f, teamNameAlpha);
				drawCenteredText(font_id, "JERALD", -0.07f, 2.0f, 0.0f, 0.0f, teamNameAlpha);
			}

			local_time += delta_time;
		}
		else {
			mainMenu_flag = TRUE;
		}

	}
}

void MainMenu_Draw() {

	if (mainMenu_flag == TRUE) {
		AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f); //TEST DRAW BACKGRUNG HERE

		//ANIMATION______________________________
		ANIMATION::background.Anim_Draw(ASSETS::backgroundAssets); //Draws BACKGROUND
		drawQuad(background,
			parallax_x * 0.3f,
			parallax_y * 0.3f,
			window_width,
			window_height,
			1.f, 1.f, 1.f, 0.7f);
		ANIMATION::gameLogo.Anim_Draw(ASSETS::brandAssets); //Draws GAMELOGO

		drawQuad(gameLogo,
			parallax_x * 0.8f,
			250.0f + parallax_y * 0.8f,
			350.0f,
			350.0f,
			1.f, 1.f, 1.f, 1.f);
		//UI BUTTONS_____________________________
		UI::startBtn.UI_Draw(ASSETS::UIAssets);
		UI::shopBtn.UI_Draw(ASSETS::UIAssets);
		UI::exitBtn.UI_Draw(ASSETS::UIAssets);
		UI::lvlEditBtn.UI_Draw(ASSETS::UIAssets);
		UI::highscoreBtn.UI_Draw(ASSETS::UIAssets);
		UI::settingsBtn.UI_Draw(ASSETS::UIAssets);
		UI::creditsBtn.UI_Draw(ASSETS::UIAssets);
		UI::lvlSelectorBtn.UI_Draw(ASSETS::UIAssets);
		//---------------------------------------

		// Button label on hover
		// Get mouse position in world space
		s32 mouseX_int{}, mouseY_int{};
		AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

		// Convert screen coords to AEGfx world coords
		f32 mouseX = static_cast<f32>(mouseX_int) - (window_width * 0.5f);
		f32 mouseY = -(static_cast<f32>(mouseY_int) - (window_height * 0.5f));

		bool hoverStart = UI::startBtn.UI_IsHovered(mouseX, mouseY);		//Using default values in class
		bool hoverShop = UI::shopBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverLvlEdit = UI::lvlEditBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverHighscore = UI::highscoreBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverSettings = UI::settingsBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverExit = UI::exitBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverCredits = UI::creditsBtn.UI_IsHovered(mouseX, mouseY);
		bool hoverLvlSelector = UI::lvlSelectorBtn.UI_IsHovered(mouseX, mouseY);

		if (hoverStart)     UI::startBtn.UI_DrawHoverText(font_id, "START (ENTER)");
		if (hoverShop)      UI::shopBtn.UI_DrawHoverText(font_id, "SHOP (S)");
		if (hoverLvlEdit)   UI::lvlEditBtn.UI_DrawHoverText(font_id, "LEVEL EDIT (E)");
		if (hoverHighscore) UI::highscoreBtn.UI_DrawHoverText(font_id, "HIGH SCORES (H)");
		if (hoverSettings)  UI::settingsBtn.UI_DrawHoverText(font_id, "SETTINGS (T)");
		if (hoverExit)      UI::exitBtn.UI_DrawHoverText(font_id, "EXIT (ESC)");
		if (hoverCredits)     UI::creditsBtn.UI_DrawHoverText(font_id, "CREDITS (C)");
		if (hoverLvlSelector) UI::lvlSelectorBtn.UI_DrawHoverText(font_id, "LEVEL SELECT (I)");

		if (levelSelectOpen) {
			// Panel
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			drawQuad(unit_square, 0.0f, 50.0f, 450.0f, 420.0f, 0.06f, 0.07f, 0.09f, 0.97f);

			drawCenteredText(font_id, "SELECT A LEVEL", 0.55f, 1.0f);
			drawCenteredText(font_id, "UP/DOWN   ENTER TO PLAY   ESC CANCEL", -0.65f, 0.45f);

			// Scroll indicator
			if (levelScrollOffset > 0) {
				drawCenteredText(font_id, "^ more(W) ^", 0.42f, 0.6f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
			}
			if (levelScrollOffset + k_visible_rows < (int)level_files.size()) {
				drawCenteredText(font_id, "v more(S) v", -0.15f, 0.6f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
			}

			// Mouse position for hover
			s32 mx{}, my{};
			AEInputGetCursorPosition(&mx, &my);
			mouseX = static_cast<f32>(mx) - (window_width * 0.5f);
			mouseY = -(static_cast<f32>(my) - (window_height * 0.5f));

			f32 list_top = 150.0f;
			f32 row_height = 35.0f;
			f32 panel_half_w = 200.0f;

			int visible_count = min((int)level_files.size() - levelScrollOffset, k_visible_rows);

			for (int i = 0; i < visible_count; ++i) {
				int actual_index = i + levelScrollOffset;
				f32 row_y = list_top - i * row_height;
				f32 norm_y = row_y / (window_height * 0.5f);

				bool isKeySelected = (actual_index == levelSelectCursor);
				bool isHovered = (mouseX >= -panel_half_w && mouseX <= panel_half_w &&
					mouseY >= row_y - row_height * 0.5f &&
					mouseY <= row_y + row_height * 0.5f);

				if (isKeySelected) {
					// Keyboard selected � blue bar
					drawQuad(unit_square, 0.0f, row_y, 420.0f, 30.0f, 0.2f, 0.3f, 0.6f, 1.0f);
				}
				else if (isHovered) {
					// Mouse hover � lighter bar
					drawQuad(unit_square, 0.0f, row_y, 420.0f, 30.0f, 0.15f, 0.2f, 0.35f, 1.0f);
					levelSelectCursor = actual_index; // sync keyboard cursor to hover
				}

				char name_buf[64];
				sprintf_s(name_buf, "%s", level_files[actual_index].c_str());

				f32 r = (isKeySelected || isHovered) ? 1.0f : 0.7f;
				f32 g = (isKeySelected || isHovered) ? 1.0f : 0.7f;
				f32 b = (isKeySelected || isHovered) ? 1.0f : 0.7f;
				drawCenteredText(font_id, name_buf, norm_y - 0.02f, 0.75f, 0.0f, 0.0f, r, g, b, 1.0f);
			}
		}

		if (quitting_flag == TRUE) {
			drawCenteredText(font_id, "ARE YOU SURE? (Y/N)", -0.8f, 0.9f);
		}
		else {
			drawCenteredText(font_id, "WASD / ARROWS TO MOVE", -0.8f, 0.9f);
		}

		if (tutorialPromptOpen){

			AEGfxSetBlendMode(AE_GFX_BM_BLEND);
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
			AEGfxSetTransparency(1.0f);

			// Dark overlay
			drawQuad(unit_square, 0.0f, 0.0f, window_width, window_height, 0.0f, 0.0f, 0.0f, 0.55f);

			// Panel as plain dark quad
			drawQuad(unit_square, 0.0f, -20.0f, 520.0f, 300.0f, 0.08f, 0.09f, 0.12f, 0.95f);

			UI::yesBtn.UI_Draw(ASSETS::UIAssets);
			UI::noBtn.UI_Draw(ASSETS::UIAssets);

			// Text - set render mode AFTER all quad draws
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
			AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
			AEGfxSetTransparency(1.0f);

			drawCenteredText(font_id, "Do you need a tutorial?",
				40.0f / (window_height * 0.5f),
				1.3f,
				0.0f, 0.0f,
				1.f, 1.f, 1.f, 1.f);

			drawCenteredText(font_id, "YES",
				-160.0f / (window_height * 0.5f),   // yesBtn posY normalized
				1.2f,
				-150.0f / (window_width * 0.5f),   // yesBtn posX normalized
				0.0f,
				1.f, 1.f, 1.f, 1.f);

			drawCenteredText(font_id, "NO",
				-160.0f / (window_height * 0.5f),   // noBtn posY normalized
				1.2f,
				150.0f / (window_width * 0.5f),    // noBtn posX normalized
				0.0f,
				1.f, 1.f, 1.f, 1.f);
		}
	}
}

void MainMenu_Free() {
	local_time = 0.0f; //Reset timer
	tutorialPromptOpen = FALSE;

    if (unit_square) { AEGfxMeshFree(unit_square);  unit_square = nullptr; }
    if (gameLogo)    { AEGfxMeshFree(gameLogo);      gameLogo    = nullptr; }
    if (background)  { AEGfxMeshFree(background);    background  = nullptr; }


	// Custom Level Selector mem cleanup
	level_files.clear();
	level_files.shrink_to_fit();
	levelSelectOpen = false;
	levelSelectCursor = 0;
	levelScrollOffset = 0;
	// ========================
}
void MainMenu_Unload() {
	UI::startBtn.UI_Free();
	UI::shopBtn.UI_Free();
	UI::exitBtn.UI_Free();
	UI::lvlEditBtn.UI_Free();
	UI::highscoreBtn.UI_Free();
	UI::settingsBtn.UI_Free();
	UI::creditsBtn.UI_Free();
	UI::lvlSelectorBtn.UI_Free();
	UI::yesBtn.UI_Free();
	UI::noBtn.UI_Free();

	ASSETS::Unload_Images();
	ASSETS::Unload_Font();
}


// For Custom Level Selection, scans MapLevel directory for ExportedLevelN.txt files and populates level_files vector with their names (without .txt)
static void ScanLevelFiles(std::vector<std::string>& level_files_vect)
{
	level_files_vect.clear();
	namespace fs = std::filesystem;

	if (!fs::exists("MapLevel")) return;

	// Collect all .txt files named ExportedLevelN
	for (int i = 1; ; ++i) {
		std::string path = "MapLevel/ExportedLevel" + std::to_string(i) + ".txt";
		if (!fs::exists(path)) break; // stop at first gap
		level_files_vect.push_back("ExportedLevel" + std::to_string(i));
	}
}
