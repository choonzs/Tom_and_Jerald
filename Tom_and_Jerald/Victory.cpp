#include "pch.hpp"
#include "Victory.hpp"
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"

namespace {
    // This instance lives only within this file
    VictoryState g_victory;
}

// --- Class Method Implementations ---

void VictoryState::Load() {
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void VictoryState::Initialize() {
    createUnitSquare(&unit_square);
    AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(nullptr, 0.0f, 0.0f);
    AEGfxSetCamPosition(0.0f, 0.0f);
}

void VictoryState::Update() {
    if (AEInputCheckTriggered(AEVK_N)) {
        PlayClick();
        next = GAME_STATE_PLAYING;
    }
    else if (AEInputCheckTriggered(AEVK_M)) {
        PlayClick();
        next = GAME_STATE_MENU;
    }
    else if (AEInputCheckTriggered(AEVK_S)) {
        PlayClick();
        next = GAME_STATE_SHOP;
    }
    else if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
        PlayClick();
        next = GAME_STATE_QUIT;
    }
}

void VictoryState::Draw() {
    drawCenteredText(font_id, "CONGRATULATIONS!", 0.35f, 1.0f);
    drawCenteredText(font_id, "PROCEED TO NEXT STAGE (N)", 0.05f, 0.7f);
    drawCenteredText(font_id, "SAVE AND RETURN TO MAIN MENU (M)", -0.1f, 0.7f);
    drawCenteredText(font_id, "SHOP (S)", -0.2f, 0.7f);
    drawCenteredText(font_id, "EXIT (ESC)", -0.3f, 0.7f);
}

void VictoryState::Free() {
    if (unit_square) {
        AEGfxMeshFree(unit_square);
        unit_square = nullptr;
    }
}

void VictoryState::Unload() {
    if (font_id != -1) {
        AEGfxDestroyFont(font_id);
        font_id = -1;
    }
}

// --- Global Wrapper Functions ---
// These are what the GameStateManager actually calls.

void Victory_Load() { g_victory.Load(); }
void Victory_Initialize() { g_victory.Initialize(); }
void Victory_Update() { g_victory.Update(); }
void Victory_Draw() { g_victory.Draw(); }
void Victory_Free() { g_victory.Free(); }
void Victory_Unload() { g_victory.Unload(); }