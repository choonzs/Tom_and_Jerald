#include "pch.hpp"
#include "CreditsScreen.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "ImgFontInit.hpp"

namespace {
    s8 font_id = -1;
    AEGfxVertexList* unit_square = nullptr;

    // Scroll state
    f32 scroll_y     = 0.0f;
    f32 scroll_speed = 60.0f;  // pixels per second

    // Copyright logo dimensions
    const f32 COPYRIGHT_W = 420.0f;
    const f32 COPYRIGHT_H = 80.0f;
}

void CreditsScreen_Load() {
    ASSETS::Init_Images();
    ASSETS::Init_Font();
    font_id = ASSETS::Font();
}

void CreditsScreen_Initialize() {
    createUnitSquare(&unit_square);
    scroll_y = 0.0f;
}

void CreditsScreen_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
        next = GAME_STATE_MENU;
    }

    f32 dt = (f32)AEFrameRateControllerGetFrameTime();
    scroll_y += scroll_speed * dt;
}

void CreditsScreen_Draw() {
    AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
    AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.f);

    f32 halfH = AEGfxGetWinMaxY();

    // ------------------------------------------------------------------
    // 1. DigiPen copyright – always pinned at the very top of the screen
    //    (requirement: must appear at top/beginning, NOT end of scroll)
    // ------------------------------------------------------------------
    f32 copyright_y = halfH - COPYRIGHT_H * 0.5f - 10.0f;
    if (ASSETS::copyrightText) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        drawQuad(unit_square, 0.0f, copyright_y, COPYRIGHT_W, COPYRIGHT_H, 1.f, 1.f, 1.f, 1.f);
        AEGfxTextureSet(ASSETS::copyrightText, 0.0f, 0.0f);

        AEMtx33 scale, trans, transform;
        AEMtx33Scale(&scale, COPYRIGHT_W, COPYRIGHT_H);
        AEMtx33Trans(&trans, 0.0f, copyright_y);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
        AEGfxMeshDraw(unit_square, AE_GFX_MDM_TRIANGLES);
    }

    // ------------------------------------------------------------------
    // 2. Scrolling credits text (starts below the copyright bar)
    // ------------------------------------------------------------------
    // Each line is placed relative to a base Y that moves upward over time.
    // scroll_y increases → credits scroll upward.
    f32 base_y    = halfH - COPYRIGHT_H - 30.0f - scroll_y;
    f32 line_gap  = 45.0f;
    f32 col1      = -0.95f;   // left-aligned normalised X for section headers
    f32 title_s   = 0.8f;
    f32 name_s    = 0.6f;
    f32 header_s  = 0.7f;

    // Helper: convert world-space Y to the normalised [-1,1] range AEGfxPrint expects
    f32 halfH_f   = halfH;
    auto worldToNorm = [&](f32 wy) -> f32 { return wy / halfH_f; };

    // ---- TITLE ----
    drawCenteredText(font_id, "C R E D I T S", worldToNorm(base_y), title_s,
                     0.0f, 0.0f, 1.0f, 0.85f, 0.2f, 1.0f);

    base_y -= line_gap * 1.5f;

    // ---- PRESIDENT ----
    AEGfxPrint(font_id, "President", col1, worldToNorm(base_y), header_s,
               0.9f, 0.6f, 0.1f, 1.0f);
    base_y -= line_gap;
    AEGfxPrint(font_id, "  Claude Comair", col1, worldToNorm(base_y), name_s,
               1.0f, 1.0f, 1.0f, 1.0f);

    base_y -= line_gap * 1.5f;

    // ---- INSTRUCTORS ----
    AEGfxPrint(font_id, "Instructors", col1, worldToNorm(base_y), header_s,
               0.9f, 0.6f, 0.1f, 1.0f);
    base_y -= line_gap;
    AEGfxPrint(font_id, "  Gerald Wong",    col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Tommy Tan",      col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Soroor",         col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Prasanna Ghali", col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f);

    base_y -= line_gap * 1.5f;

    // ---- TEAM MEMBERS ----
    AEGfxPrint(font_id, "Team Members", col1, worldToNorm(base_y), header_s,
               0.9f, 0.6f, 0.1f, 1.0f);
    base_y -= line_gap;
    AEGfxPrint(font_id, "  Tan Choon Ming",       col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Dan Ng Cher Kai",       col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Peng Rong Jun Jerald",  col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Loh Kai Xin",           col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f); base_y -= line_gap;
    AEGfxPrint(font_id, "  Ong Jin Ting",           col1, worldToNorm(base_y), name_s, 1.f, 1.f, 1.f, 1.f);

    base_y -= line_gap * 2.0f;

    // ---- FOOTER ----
    drawCenteredText(font_id, "Press ESC to return", worldToNorm(base_y), 0.5f,
                     0.0f, 0.0f, 0.6f, 0.6f, 0.6f, 1.0f);

    // Reset render mode
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.f, 0.f);
}

void CreditsScreen_Free() {
    if (unit_square) { AEGfxMeshFree(unit_square); unit_square = nullptr; }
}

void CreditsScreen_Unload() {
    ASSETS::Unload_Font();
    ASSETS::Unload_Images();
}
