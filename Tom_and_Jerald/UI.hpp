#pragma once //Header guard
#include"pch.hpp"
#include "Animation.hpp"

namespace UI {

	class UIButtons {
	public: //Enum members______________________________
		enum class buttonKey { //Needs to be set in spritesheet order
			blank = 0, 
			pause = 1, 
			start = 2, 
			shop = 3,
			lvlEditor = 4, 
			highscore = 5, 
			settings = 6,
			exit = 7, 
			back = 8
		};//End enum class buttons

	public: //Data members______________________________
		f32 posX{ 0.0f };
		f32 posY{ 0.0f };
		f32 width{ 50.0f };
		f32 height{ 50.0f };
		int key{ 0 };
		AEGfxVertexList* mesh{ nullptr };//The quad to draw on, defaulted to nullptr
		ANIMATION::AnimatedSprite sprite; //The sprite to draw

	public: //Member functions__________________________
		//Initialize buttons
		void UI_Init(f32 posX, f32 posY, f32 width, f32 height);


		//Set up specific button
		void UI_Select(buttonKey button_type);		//Takes in enum to choose button type


		//Renders the button
		void UI_Draw(AEGfxTexture* texture) const;

		// Explicitly free mesh (required when changing screens, otherwise memory leak)
		void UI_Free();

		//Destructor to free mesh
		~UIButtons();
	};//End class UIButtons
	
	//Instances
	extern UIButtons startBtn;
	extern UIButtons shopBtn;
	extern UIButtons exitBtn;
	extern UIButtons lvlEditBtn;
	extern UIButtons highscoreBtn;
	extern UIButtons settingsBtn;


}//End namespace UI