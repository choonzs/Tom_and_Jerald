#include "pch.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Settings.hpp"
#include "Audio.hpp"

// slider values
f32 bgmVolume = 0.5f;   // default bgm volume
f32 seVolume = 0.5f;   // default se volume
f32 sliderX_bgm = 0.5f;  // default bgm slider position (middle)
f32 sliderX_se = 0.5f;  // default se slider position (middle)

AEGfxVertexList* sliderTrackMesh = nullptr; // the bar background
AEGfxVertexList* sliderHandleMesh = nullptr; // the draggable handle

namespace {
    s8 font_id;
}

void Setting_Load() {
    // load font for settings screen
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void Settings_Initialize() {
    // reset sliders to default values when entering settings
    sliderX_bgm = 0.5f;
    sliderX_se = 0.5f;

    // create slider track mesh
    // a flat rectangle used as the slider bar background
    AEGfxMeshStart();
    AEGfxTriAdd(
        -1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f
    );
    AEGfxTriAdd(
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f
    );
    sliderTrackMesh = AEGfxMeshEnd();

    // create slider handle mesh using full -1 to 1 range
    // a flat rectangle used as the slider bar background
    AEGfxMeshStart();
    AEGfxTriAdd(
        -1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f
    );
    AEGfxTriAdd(
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f
    );
    sliderHandleMesh = AEGfxMeshEnd();
}

void Settings_Update() {

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    // convert mouse position to world space
    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();
    f32 mouseX = (f32)mx - halfW;
    f32 mouseY = -(f32)my + halfH;

    // track if mouse was just clicked this frame
    bool justClicked = AEInputCheckTriggered(AEVK_LBUTTON);

    if (AEInputCheckCurr(AEVK_LBUTTON))
    {
        // BGM slider
        // check if mouse is within the bgm slider track area
        if (mouseY >= 25.f && mouseY <= 75.f)
        {
            // play click only on initial press, not while dragging
            if (justClicked) PlayClick();
            // clamp mouse to slider range and normalize to 0.0 - 1.0
            sliderX_bgm = (AEClamp(mouseX, -300.f, 300.f) + 300.f) / 600.f;
            bgmVolume = sliderX_bgm;
            AEAudioSetGroupVolume(bgm, bgmVolume);
        }

        // SE slider
        // check if mouse is within the se slider track area
        if (mouseY >= -75.f && mouseY <= -25.f)
        {
            // play click only on initial press, not while dragging
            if (justClicked) PlayClick();
            // clamp mouse to slider range and normalize to 0.0 - 1.0
            sliderX_se = (AEClamp(mouseX, -300.f, 300.f) + 300.f) / 600.f;
            seVolume = sliderX_se;
            AEAudioSetGroupVolume(se_click, seVolume); // apply volume to click group
            AEAudioSetGroupVolume(se_rat, seVolume); // apply volume to rat group
        }
    }

    // back to menu
    if (AEInputCheckTriggered(AEVK_ESCAPE))
    {
        PlayClick();
        next = GAME_STATE_MENU;
    }
}

void Settings_Draw() {

    AEMtx33 transform, scale, trans;

    // set render mode to color only, no texture
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(1.f);

    // draw BGM slider track
    // flat white bar centered
    AEMtx33Scale(&scale, 300.f, 2.f);
    AEMtx33Trans(&trans, 0.f, 50.f);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(sliderTrackMesh, AE_GFX_MDM_TRIANGLES);

    // draw BGM slider handle
    // maps sliderX_bgm (0.0 to 1.0) to screen space (-300 to 300)
    f32 handlePosX_bgm = (sliderX_bgm * 2.f - 1.f) * 300.f;
    AEMtx33Scale(&scale, 8.f, 15.f);
    AEMtx33Trans(&trans, handlePosX_bgm, 50.f);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(sliderHandleMesh, AE_GFX_MDM_TRIANGLES);

    // draw SE slider track
    // flat white bar centered
    AEMtx33Scale(&scale, 300.f, 2.f);
    AEMtx33Trans(&trans, 0.f, -50.f);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(sliderTrackMesh, AE_GFX_MDM_TRIANGLES);

    // draw SE slider handle
    // maps sliderX_se (0.0 to 1.0) to screen space (-300 to 300)
    f32 handlePosX_se = (sliderX_se * 2.f - 1.f) * 300.f;
    AEMtx33Scale(&scale, 8.f, 15.f);
    AEMtx33Trans(&trans, handlePosX_se, -50.f);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(sliderHandleMesh, AE_GFX_MDM_TRIANGLES);

    // draw labels
    // display volume as percentage (0% to 100%)
    char bgmBuffer[32], seBuffer[32];
    sprintf_s(bgmBuffer, 32, "%.0f%%", bgmVolume * 100.f);  // shows 100% instead of 1.0
    sprintf_s(seBuffer, 32, "%.0f%%", seVolume * 100.f);

    AEGfxPrint(font_id, "BGM Volume:", -0.8f, 0.2f, 0.5f, 1, 1, 1, 1);
    AEGfxPrint(font_id, bgmBuffer, 0.7f, 0.2f, 0.5f, 1, 1, 1, 1);
    AEGfxPrint(font_id, "SFX Volume:", -0.8f, -0.1f, 0.5f, 1, 1, 1, 1);
    AEGfxPrint(font_id, seBuffer, 0.7f, -0.1f, 0.5f, 1, 1, 1, 1);
}

void Settings_Free() {
    // free meshes to prevent memory leaks
    AEGfxMeshFree(sliderTrackMesh);
    AEGfxMeshFree(sliderHandleMesh);
}

void Settings_Unload() {

}