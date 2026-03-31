#include "pch.hpp"
#include "UI.hpp"
#include "Animation.hpp"
#include "Utils.hpp"

AEGfxVertexList* unit_square = nullptr; //Create mesh to put UI
createUnitSquare(&unit_square, 0.25f, 0.25f);

	//Initialize buttons
	void UIButtonsUI_Init(f32 posX, f32 posY) {
		//Create all instances here
		ANIMATION::gameLogo.ImportFromFile("Assets/AnimationData.txt");			//Total rows + columns file
		ANIMATION::gameLogo.Clip_Select(0, 0, 4, 5.0f);							//Row, start col, frames, fps (GAMELOGO)

	}


	//Set up specific button
	void UIButtons::UI_Select(u32 row, u32 start_col, u32 frame_count, f32 fps = 2.0f) {
		//Select based on enums
		

	}

	//Updates animation clip to run
	void UIButtons::UI_Update(f32 delta_time) {			//Not sure if i need this

	}

	//Renders the clip
	void UIButtons::UI_Draw(AEGfxTexture* Texture) const {

	}
