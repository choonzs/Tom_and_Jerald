#include "pch.hpp"
#include "UI.hpp"
#include "Animation.hpp"
#include "Utils.hpp"
namespace UI {
    //Instances
    UIButtons startBtn;
    UIButtons shopBtn;
    UIButtons exitBtn;

    void UIButtons::UI_Init(f32 x, f32 y, f32 w, f32 h) {
        posX = x;
        posY = y;
        width = w;
        height = h;
        createUnitSquare(&mesh, 0.25f, 0.25f); //create mesh here, not globally
        sprite.ImportFromFile("Assets/AnimationData.txt");  // load spritesheet layout
    }
    void UIButtons::UI_Select(buttonKey button_type) {
        key = static_cast<int>(button_type); //convert enum to int, store in key // e.g. start = 2
        u32 col = key % 4;   // which column on the sheet // 2 % 4 = 2 ->  column 2
        u32 row = key / 4;   // which row on the sheet // 2 / 4 = 0 ->  row 0
        sprite.Clip_Select(row, col, 1);  // 1 frame = static, no animation
    }
    void UIButtons::UI_Draw(AEGfxTexture* texture) const {
        sprite.Anim_Draw(texture);                          // sets the UV
        drawQuad(mesh, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);
    }

	// Needed to free mesh when changing screens, otherwise memory leak, also called in destructor
    void UIButtons::UI_Free() {
        if (mesh) {
            AEGfxMeshFree(mesh);
            mesh = nullptr;
		}
    }
    //Destructor to free mesh
    UIButtons::~UIButtons() {
        if (mesh) {
            AEGfxMeshFree(mesh);
            mesh = nullptr;
        }
    }
}//End namespace UI