/*************************************************************************
@file    CreditsScreen.cpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  Loh Kai Xin kaixin.l@digipen.edu
@brief
     Implements the scrolling Credits screen: parses entries from
     CreditsContent.txt and renders them with auto-scroll animation.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#include "pch.hpp"
#include "CreditsScreen.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Utils.hpp"
#include "ImgFontInit.hpp"
#include "Animation.hpp"
#include "UI.hpp"

namespace {
    s8 font_id = -1;
    AEGfxVertexList* unit_square = nullptr;
    AEGfxVertexList* background = nullptr;			//For background

    // Scroll state — loaded from CreditsContent.txt
    f32 scroll_y     = 0.0f;
    f32 scroll_speed = 60.0f;
    f32 g_line_gap   = 45.0f;

    // Copyright logo dimensions
    const f32 COPYRIGHT_W = 420.0f;
    const f32 COPYRIGHT_H = 80.0f;

    f32 dt;

    f32 window_width;
    f32 window_height;

    // ---------------------------------------------------------------------------
    // Credits content — loaded from Assets/data/CreditsContent.txt
    // ---------------------------------------------------------------------------
    struct CreditsEntry {
        enum class Type { Title, Section, Name, Footer, Spacer } type;
        std::string text;
    };
    std::vector<CreditsEntry> g_credits_entries;

    void LoadCreditsFromFile(const std::string& filename)
    {
        g_credits_entries.clear();
        scroll_speed = 60.0f;   // reset to safe defaults before reading
        g_line_gap   = 45.0f;

        std::ifstream f(filename);
        if (!f.is_open()) {
            // File missing — use minimal hardcoded fallback so screen still works
            CreditsEntry e;
            e.type = CreditsEntry::Type::Title;  e.text = "C R E D I T S";
            g_credits_entries.push_back(e);
            e.type = CreditsEntry::Type::Spacer; e.text = "";
            g_credits_entries.push_back(e);
            e.type = CreditsEntry::Type::Footer; e.text = "Press ESC to return";
            g_credits_entries.push_back(e);
            return;
        }

        std::string line;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;

            // Parse key-value parameters first
            std::istringstream ss(line);
            std::string first;
            ss >> first;
            if (first == "scroll_speed") { ss >> scroll_speed; continue; }
            if (first == "line_gap")     { ss >> g_line_gap;   continue; }

            // SPACER needs no text
            if (first == "SPACER") {
                CreditsEntry entry;
                entry.type = CreditsEntry::Type::Spacer;
                entry.text = "";
                g_credits_entries.push_back(entry);
                continue;
            }

            // All other content uses TYPE|text format
            auto sep = line.find('|');
            if (sep == std::string::npos) continue;
            std::string type_str = line.substr(0, sep);
            std::string text     = line.substr(sep + 1);

            CreditsEntry entry;
            if      (type_str == "TITLE")   entry.type = CreditsEntry::Type::Title;
            else if (type_str == "SECTION") entry.type = CreditsEntry::Type::Section;
            else if (type_str == "NAME")    entry.type = CreditsEntry::Type::Name;
            else if (type_str == "FOOTER")  entry.type = CreditsEntry::Type::Footer;
            else continue;  // unknown type — skip

            entry.text = text;
            g_credits_entries.push_back(entry);
        }
    }
}

void CreditsScreen_Load() {
    ASSETS::Init_Images();
    ASSETS::Init_Font();
    font_id = ASSETS::Font();
}

void CreditsScreen_Initialize() {
    createUnitSquare(&unit_square);
    createUnitSquare(&background, 0.25f, 0.25f);
    scroll_y = 0.0f;
    LoadCreditsFromFile("Assets/data/CreditsContent.txt");

    window_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    window_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

    ANIMATION::background.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::background.Clip_Select(0, 0, 4, 2.0f);						//For background
}

void CreditsScreen_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
        gSkipSplash = true;
        next = GAME_STATE_MENU;
    }

    dt = (f32)AEFrameRateControllerGetFrameTime();
    scroll_y -= scroll_speed * dt;

    ANIMATION::background.Anim_Update(dt);
}

void CreditsScreen_Draw() {
    AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
    AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.f);

    ANIMATION::background.Anim_Draw(ASSETS::backgroundAssets); //Draws BACKGROUND
    drawQuad(background,
        0.0f,
        0.0f,
        window_width,
        window_height,
        1.f, 1.f, 1.f, 0.5f);

    f32 halfH = AEGfxGetWinMaxY();

    // ------------------------------------------------------------------
    // 1. DigiPen copyright — always pinned at the very top of the screen
    // ------------------------------------------------------------------
    f32 copyright_y = halfH - COPYRIGHT_H * 0.5f - 10.0f;
    if (ASSETS::copyrightText) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(ASSETS::copyrightText, 0.0f, 0.0f);
        drawQuad(unit_square, 0.0f, copyright_y, COPYRIGHT_W, COPYRIGHT_H, 1.f, 1.f, 1.f, 1.f);
    }

    // ------------------------------------------------------------------
    // 2. Scrolling credits text — driven by Assets/data/CreditsContent.txt
    // ------------------------------------------------------------------
    f32 base_y   = halfH - COPYRIGHT_H - 30.0f - scroll_y;
    f32 line_gap = g_line_gap;
    f32 col1     = -0.95f;
    f32 title_s  = 1.2f;
    f32 name_s   = 0.9f;
    f32 header_s = 1.0f;
    f32 halfH_f  = halfH;
    auto worldToNorm = [&](f32 wy) -> f32 { return wy / halfH_f; };

    for (auto& entry : g_credits_entries) {
        switch (entry.type) {
        case CreditsEntry::Type::Title:
            drawCenteredText(font_id, entry.text.c_str(),
                             worldToNorm(base_y), title_s,
                             0.0f, 0.0f, 1.0f, 0.85f, 0.2f, 1.0f);
            base_y -= line_gap;
            break;

        case CreditsEntry::Type::Section:
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxPrint(font_id, entry.text.c_str(),
                       col1, worldToNorm(base_y), header_s,
                       0.9f, 0.6f, 0.1f, 1.0f);
            base_y -= line_gap;
            break;

        case CreditsEntry::Type::Name:
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxPrint(font_id, entry.text.c_str(),
                       col1, worldToNorm(base_y), name_s,
                       1.0f, 1.0f, 1.0f, 1.0f);
            base_y -= line_gap;
            break;

        case CreditsEntry::Type::Footer:
            drawCenteredText(font_id, entry.text.c_str(),
                             worldToNorm(base_y), 0.8f,
                             0.0f, 0.0f, 0.6f, 0.6f, 0.6f, 1.0f);
            // No decrement — footer is the last entry
            break;

        case CreditsEntry::Type::Spacer:
            base_y -= line_gap * 0.5f;
            break;
        }
    }

    // Reset render mode
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.f, 0.f);
}

void CreditsScreen_Free() {
    if (unit_square) { AEGfxMeshFree(unit_square); unit_square = nullptr; }
    if (background) { AEGfxMeshFree(background);  background = nullptr; }
    g_credits_entries.clear();
}

void CreditsScreen_Unload() {
    ASSETS::Unload_Font();
    ASSETS::Unload_Images();
}
