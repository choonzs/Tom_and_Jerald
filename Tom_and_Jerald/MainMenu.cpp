#include "pch.hpp"

// For testing
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "Camera.hpp"
#include "Audio.hpp"
#include "ImgFontInit.hpp"
#include "Animation.hpp"
#include "UI.hpp"

namespace {
	s8 font_id; // Reference to the font ID created in ImgFontInit, used for drawing text in this scene
	AEGfxVertexList* unit_square = nullptr;			//For Splashscreen drawing
	AEGfxVertexList* gameLogo = nullptr;			//For Game logo
	AEGfxVertexList* background = nullptr;			//For background

	int counter{};
	f32 cam_pos_x, cam_pos_y, cam_pos_angle{};
	Camera camera;

	f32 delta_time;
	f32 local_time;

	f32 window_width;								//Window Width
	f32 window_height;								//Window Height

	BOOL mainMenu_flag;								//For main menu access, plays after teamname splashscreen
	BOOL teamName_flag;								//For teamname splashscreen, plays after copyright splashscreen
	BOOL quitting_flag; // Used to trigger quitting the game destructive action when player clicks escape or closes window


	// txt files start w 1
	unsigned int select_level{ 1 };

	// Testing parallax using mouse
	f32 parallax_x = 0.0f;
	f32 parallax_y = 0.0f;
}


//LOAD
void MainMenu_Load() {
	//font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	ASSETS::Init_Images();
	ASSETS::Init_Font();
	font_id = ASSETS::Font();
}

//INITIALIZE
void MainMenu_Initialize() {
	createUnitSquare(&unit_square);
	createUnitSquare(&gameLogo, 0.25f, 0.25f);
	createUnitSquare(&background, 0.25f, 0.25f);

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);

	AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

	delta_time = (f32)AEFrameRateControllerGetFrameTime();

	window_width = (AEGfxGetWinMaxX() - AEGfxGetWinMinX());
	window_height = (AEGfxGetWinMaxY() - AEGfxGetWinMinY());

	mainMenu_flag = FALSE; //False on default, triggers main menu after teamname
	teamName_flag = FALSE; //False on defualt, triggers after splashscreen

	quitting_flag = FALSE; //False on default, triggers when player clicks escape or closes window

	//ANIMATION______________________________
	ANIMATION::gameLogo.ImportFromFile("Assets/AnimationData.txt");			//Total rows + columns file
	ANIMATION::gameLogo.Clip_Select(0, 0, 4, 5.0f);							//Row, start col, frames, fps (GAMELOGO)

	ANIMATION::background.ImportFromFile("Assets/AnimationData.txt");		//Total rows + columns file
	ANIMATION::background.Clip_Select(0, 0, 4, 2.0f);						//Row, start col, frames, fps (BACKGROUND)
	//UI BUTTONS-----------------------------
	// Top row: start, shop, lvlEditor
	UI::startBtn.UI_Init(-325.0f, 50.0f, 200.0f, 200.0f);
	UI::startBtn.UI_Select(UI::UIButtons::buttonKey::start);

	UI::shopBtn.UI_Init(0.0f, 50.0f, 200.0f, 200.0f);
	UI::shopBtn.UI_Select(UI::UIButtons::buttonKey::shop);

	UI::lvlEditBtn.UI_Init(325.0f, 50.0f, 200.0f, 200.0f);
	UI::lvlEditBtn.UI_Select(UI::UIButtons::buttonKey::lvlEditor);

	// Bottom row: highscore, settings, exit
	UI::highscoreBtn.UI_Init(-325.0f, -200.0f, 200.0f, 200.0f);
	UI::highscoreBtn.UI_Select(UI::UIButtons::buttonKey::highscore);

	UI::settingsBtn.UI_Init(0.0f, -200.0f, 200.0f, 200.0f);
	UI::settingsBtn.UI_Select(UI::UIButtons::buttonKey::settings);

	UI::exitBtn.UI_Init(325.0f, -200.0f, 200.0f, 200.0f);
	UI::exitBtn.UI_Select(UI::UIButtons::buttonKey::exit);
	//_______________________________________

	backgroundAudio.Play();

	// Show cursor please
	AEInputShowCursor(true);
}

void MainMenu_Update() {
	camera.Update();
	
	delta_time = (f32)AEFrameRateControllerGetFrameTime();
	//ANIMATION______________________________
	ANIMATION::gameLogo.Anim_Update(delta_time);
	ANIMATION::background.Anim_Update(delta_time);
	//---------------------------------------

	// Trying something with mouse
	s32 mouseX_int{};
	s32 mouseY_int{};

	AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

	f32 nx = (static_cast<f32>(mouseX_int) / window_width) * 2.0f - 1.0f;
	f32 ny = (static_cast<f32>(mouseY_int) / window_height) * 2.0f - 1.0f;

	ny = -ny;

	f32 target_x = nx * 30.0f;
	f32 target_y = ny * 30.0f;

	parallax_x += (target_x - parallax_x) * 0.1f;
	parallax_y += (target_y - parallax_y) * 0.1f;
	// ==========================================

	/*if (IsMenuKeyTriggered()) {
		clickAudio.Play();
	}*/

	if (AEInputCheckTriggered(AEVK_LBUTTON)) { //Player clicks a button
		clickAudio.Play();
	}


	if (local_time < 3.0f) {
		teamName_flag = FALSE;

		// Fade out: full opacity at 0s, invisible at 3s
		f32 splashScreenAlpha = 1.0f - (local_time / 3.0f);

		//Default settings
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		//Runs the splashscreen
		AEGfxSetTransparency(splashScreenAlpha);

		//Draw the background
		AEGfxTextureSet(NULL, 1.0f, 1.0f);
		drawQuad(background, 0, 0.0, window_width, window_height, 1.f, 1.f, 1.f, 1.f);

		AEGfxTextureSet(ASSETS::copyrightLogo, 1.0f, 1.0f);
		drawQuad(unit_square, 0, 0, window_width - 100.0f, window_height - 100.0f, 1.f, 1.f, 1.f, splashScreenAlpha);



		local_time += delta_time;

	}
	else {
		teamName_flag = TRUE;
	}

	// Destructive confirmation for quitting the game
	if (quitting_flag == TRUE) {
		// Click Y to quit, N to cancel
		if (AEInputCheckTriggered(AEVK_Y)) {
			// Quitting the game
			next = GAME_STATE_QUIT;
		}
		else if (AEInputCheckTriggered(AEVK_N)) {
			quitting_flag = FALSE;
		}
	}
	// ===========================================

	if (mainMenu_flag == TRUE) {

		// Mouse click on buttons
		if (AEInputCheckTriggered(AEVK_LBUTTON)) {
			s32 mouseX_int{}, mouseY_int{};
			AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

			f32 mouseX = static_cast<f32>(mouseX_int) - (window_width * 0.5f);
			f32 mouseY = -(static_cast<f32>(mouseY_int) - (window_height * 0.5f));

			f32 half = 100.0f;

			bool hoverStart = (mouseX >= -325.0f - half && mouseX <= -325.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
			bool hoverShop = (mouseX >= 0.0f - half && mouseX <= 0.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
			bool hoverLvlEdit = (mouseX >= 325.0f - half && mouseX <= 325.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
			bool hoverHighscore = (mouseX >= -325.0f - half && mouseX <= -325.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);
			bool hoverSettings = (mouseX >= 0.0f - half && mouseX <= 0.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);
			bool hoverExit = (mouseX >= 325.0f - half && mouseX <= 325.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);

			if (hoverStart)     next = GAME_STATE_PLAYING;
			if (hoverShop)      next = GAME_STATE_SHOP;
			if (hoverLvlEdit)   next = GAME_STATE_LEVEL_EDITOR;
			if (hoverHighscore) next = GAME_STATE_HIGHSCORE;
			if (hoverSettings)  next = GAME_STATE_SETTINGS;
			if (hoverExit)      quitting_flag = TRUE;
		}
		//Temp for switching levels
		if (AEInputCheckTriggered(AEVK_A)) {
			if ((select_level) > 1) { --select_level; }
		}
		else if (AEInputCheckTriggered(AEVK_D)) {
			++select_level;
		}

		// Menu Controls
		if (AEInputCheckTriggered(AEVK_RETURN))
		{
			// Moving to playing state
			next = GAME_STATE_PLAYING;
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
			// Quitting the game
			//next = GAME_STATE_QUIT;
		}
		else if (AEInputCheckTriggered(AEVK_C))
		{
			// Write down text file number that we need to open 
			// shld move to util cpp to avoid clutter
			std::string filename;

			// Shorten for easier readability
			namespace fs = std::filesystem;
			// Create "MapLevel" directory if it doesn't exist
			if (fs::create_directory("MapLevel")) {
				//std::cout << "Directory 'MapLevel' created successfully.\n";
			}
			filename = "MapLevel/LoadLevel.txt";
			// Opens output file stream and writes the level number to load, trunc to remove anything inside
			std::ofstream outFile(filename, std::ios::trunc);
			if (outFile.is_open()) {
				outFile << select_level;
				outFile.close();
			}
			// --------------------------------------------------


			next = GAME_STATE_CUSTOM_PLAY;
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
		else if (AEInputCheckTriggered(AEVK_I))
		{
			next = GAME_STATE_CREDITS;
		}
		else {
			current = GAME_STATE_MENU;
			// Refreshing the next state to stay in menu
			//next = GAME_STATE_RESTART;
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
	char buffer[128];

	// Example: show current select level
	sprintf_s(buffer, "PLAY CUSTOM LEVEL %d (C)", select_level);

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
		//drawQuad(background, 0, 0, window_width, window_height, 1.f, 1.f, 1.f, 0.7f);
		ANIMATION::gameLogo.Anim_Draw(ASSETS::brandAssets); //Draws GAMELOGO

		drawQuad(gameLogo,
			parallax_x * 0.8f,
			250.0f + parallax_y * 0.8f,
			350.0f,
			350.0f,
			1.f, 1.f, 1.f, 1.f);
		//drawQuad(gameLogo, 0, 250.0, 350.0f, 350.0f, 1.f, 1.f, 1.f, 1.f);
		//UI BUTTONS_____________________________
		UI::startBtn.UI_Draw(ASSETS::UIAssets);
		UI::shopBtn.UI_Draw(ASSETS::UIAssets);
		UI::exitBtn.UI_Draw(ASSETS::UIAssets);
		UI::lvlEditBtn.UI_Draw(ASSETS::UIAssets);
		UI::highscoreBtn.UI_Draw(ASSETS::UIAssets);
		UI::settingsBtn.UI_Draw(ASSETS::UIAssets);
		//---------------------------------------

		// Button label on hover
// Get mouse position in world space
		s32 mouseX_int{}, mouseY_int{};
		AEInputGetCursorPosition(&mouseX_int, &mouseY_int);

		// Convert screen coords to AEGfx world coords
		f32 mouseX = static_cast<f32>(mouseX_int) - (window_width * 0.5f);
		f32 mouseY = -(static_cast<f32>(mouseY_int) - (window_height * 0.5f));

		// Button size half-extents (buttons are 200x200)
		f32 half = 100.0f;

		f32 labelScale = 1.0f;
		f32 labelDropY = -100.0f; // world units below button center

		bool hoverStart = (mouseX >= -325.0f - half && mouseX <= -325.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
		bool hoverShop = (mouseX >= 0.0f - half && mouseX <= 0.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
		bool hoverLvlEdit = (mouseX >= 325.0f - half && mouseX <= 325.0f + half && mouseY >= 50.0f - half && mouseY <= 50.0f + half);
		bool hoverHighscore = (mouseX >= -325.0f - half && mouseX <= -325.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);
		bool hoverSettings = (mouseX >= 0.0f - half && mouseX <= 0.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);
		bool hoverExit = (mouseX >= 325.0f - half && mouseX <= 325.0f + half && mouseY >= -200.0f - half && mouseY <= -200.0f + half);

		if (hoverStart)     drawCenteredText(font_id, "START (ENTER)", (50.0f + labelDropY) / (window_height * 0.5f), labelScale, -325.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);
		if (hoverShop)      drawCenteredText(font_id, "SHOP (S)", (50.0f + labelDropY) / (window_height * 0.5f), labelScale, 0.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);
		if (hoverLvlEdit)   drawCenteredText(font_id, "LEVEL EDIT (E)", (50.0f + labelDropY) / (window_height * 0.5f), labelScale, 325.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);
		if (hoverHighscore) drawCenteredText(font_id, "HIGH SCORES (H)", (-200.0f + labelDropY) / (window_height * 0.5f), labelScale, -325.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);
		if (hoverSettings)  drawCenteredText(font_id, "SETTINGS (T)", (-200.0f + labelDropY) / (window_height * 0.5f), labelScale, 0.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);
		if (hoverExit)      drawCenteredText(font_id, "EXIT (ESC)", (-200.0f + labelDropY) / (window_height * 0.5f), labelScale, 325.0f / (window_width * 0.5f), 0.0f, 1.f, 1.f, 1.f, 1.f);


		if (quitting_flag == TRUE) {
			drawCenteredText(font_id, "ARE YOU SURE? (Y/N)", -0.8f, 0.9f);
		}
		else {
			drawCenteredText(font_id, "WASD / ARROWS TO MOVE", -0.8f, 0.9f);
		}

	}
}

void MainMenu_Free() {
	AEGfxMeshFree(unit_square);
	AEGfxMeshFree(gameLogo);
	AEGfxMeshFree(background);
}
void MainMenu_Unload() {
	UI::startBtn.UI_Free();
	UI::shopBtn.UI_Free();
	UI::exitBtn.UI_Free();

	//AEGfxDestroyFont(font_id);
	ASSETS::Unload_Images();
	ASSETS::Unload_Font();
}