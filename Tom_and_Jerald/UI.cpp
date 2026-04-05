//---------------------------------------------------------
// file:	UI.cpp
// author:	Loh Kai Xin
// email:	kaixin.l@digipen.edu
//
// brief:	Class function definitions for UI icons' manipulation.
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------

//Header files
#include "pch.hpp"
#include "UI.hpp"
#include "Animation.hpp"
#include "Utils.hpp"

namespace UI {

    //Instances, extern in UI.hpp file
    UIButtons startBtn;
    UIButtons shopBtn;
    UIButtons menuBtn;
    UIButtons resumeBtn;
    UIButtons restartBtn;
    UIButtons yesBtn;
    UIButtons noBtn;
    UIButtons exitBtn;
    UIButtons lvlEditBtn;
    UIButtons highscoreBtn;
    UIButtons settingsBtn;
    UIButtons creditsBtn;
    UIButtons lvlSelectorBtn;

    //Initialize buttons 
    void UIButtons::UI_Init(f32 x, f32 y, f32 w, f32 h) {
        posX = x;
        posY = y;
        width = w;
        height = h;
        createUnitSquare(&mesh, 0.25f, 0.25f);                  //Create mesh here, not globally
        sprite.ImportFromFile("Assets/AnimationData.txt");      //Load spritesheet layout
    }

    //Select specific icon to use from spritesheet
    void UIButtons::UI_Select(buttonKey button_type) {
        key = static_cast<int>(button_type);                    //Convert enum to int, store in key (eg. start = 2)
        u32 col = key % 4;                                      //Which column on the sheet (eg. 2 % 4 = 2  ->  column 2)
        u32 row = key / 4;                                      //Which row on the sheet (eg. 2 / 4 = 0 ->  row 0)
        sprite.Clip_Select(row, col, 1);                        //1 frame = static, no animation
    }

    //Draw the selected icon
    void UIButtons::UI_Draw(AEGfxTexture* texture) const {
        sprite.Anim_Draw(texture);                              //Sets the UV
        drawQuad(mesh, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    //Draw text below icon on mouse hover
    void UIButtons::UI_DrawHoverText(s8 font, const char* text) const {
        drawCenteredText( font, text,
            (posY - dropY) / AEGfxGetWinMaxY(),                 //Y with offset
            scale,                                              //Scale
            posX / AEGfxGetWinMaxX(),                           //X coord normalized
            0.0f,                                               //Y coord
            1.f, 1.f, 1.f, 1.f);                                //RGBA
    }

    //Check if icon is hovered by mouse
    bool UIButtons::UI_IsHovered(f32 mouseX, f32 mouseY) const {
        return (mouseX >= posX - half && mouseX <= posX + half &&
            mouseY >= posY - half && mouseY <= posY + half);
    }

	//Needed to free mesh when changing screens, otherwise memory leak, also called in destructor
    void UIButtons::UI_Free() {
        if (mesh) {
            AEGfxMeshFree(mesh);
            mesh = nullptr;
		}
    }

    //Destructor to free mesh
    UIButtons::~UIButtons() {
        UI_Free();
    }

}//End namespace UI