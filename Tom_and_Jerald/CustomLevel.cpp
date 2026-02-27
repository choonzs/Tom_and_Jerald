#include "pch.hpp"
#include "CustomLevel.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "utils.hpp"
#include "Animation.hpp"
#include "Audio.hpp"
#include <vector>
#include <fstream>
#include <cmath>

namespace {
    // Custom Tile Struct to hold loaded map data
    struct LevelTile {
        int type; // 1 = Square, 2 = Spike
        AEVec2 pos;
        AEVec2 half_size;
    };

    std::vector<LevelTile> map_tiles;

    Player custom_player;
    Camera camera;

    AEGfxTexture* texSquare = nullptr;
    AEGfxTexture* texSpike = nullptr;
    AEGfxTexture* texBackground = nullptr;
    AEGfxVertexList* unit_square = nullptr;
    s8 font_id;

    f32 damage_timer = 0.0f;
    f32 level_end_x = 0.0f; // Track where the level ends to trigger victory
}

void CustomLevel_Load() {
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);

    // Load Textures
    texSquare = AEGfxTextureLoad("Assets/Square.png");
    texSpike = AEGfxTextureLoad("Assets/Spike.png");
    texBackground = AEGfxTextureLoad("Assets/Game_Background.png");
    custom_player.Texture() = AEGfxTextureLoad("Assets/Fairy_Rat.png");
}

void CustomLevel_Initialize() {
    createUnitSquare(&unit_square, 0.5f, 0.5f);
    createUnitSquare(&(custom_player.Mesh()), 0.5f, 0.5f);
    ANIMATION::sprite_Initialize();

    map_tiles.clear();
    damage_timer = 0.0f;

    // Reset Player
    AEVec2Set(&custom_player.Position(), 0.0f, 0.0f);
    AEVec2Set(&custom_player.Half_Size(), 20.0f, 20.0f);
    custom_player.Health() = custom_player.Config().MaxHealth();

    // ---------------------------------------------------------
    // READ EXPORTED LEVEL
    // ---------------------------------------------------------
    std::ifstream inFile("ExportedLevel.txt");
    if (inFile.is_open()) {
        int cols, rows;
        inFile >> cols >> rows;

        f32 halfW = AEGfxGetWinMaxX();
        f32 halfH = AEGfxGetWinMaxY();
        f32 TILE_SIZE = (halfH * 2.0f) / rows; // Dynamically scale to match Editor

        f32 startX = -halfW; // Start grid at left edge of screen
        f32 startY = -halfH; // Start grid at bottom edge

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int type;
                inFile >> type;
                if (type != 0) {
                    LevelTile tile;
                    tile.type = type;
                    tile.half_size.x = TILE_SIZE / 2.0f;
                    tile.half_size.y = TILE_SIZE / 2.0f;
                    tile.pos.x = startX + (c * TILE_SIZE) + tile.half_size.x;
                    tile.pos.y = startY + (r * TILE_SIZE) + tile.half_size.y;

                    map_tiles.push_back(tile);

                    if (tile.pos.x > level_end_x) {
                        level_end_x = tile.pos.x; // Track furthest tile
                    }
                }
            }
        }
        inFile.close();
    }
}

void CustomLevel_Update() {
    f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();

    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        PlayClick();
        next = GAME_STATE_MENU;
        return;
    }

    if (damage_timer > 0.0f) {
        damage_timer -= delta_time;
    }

    // 1. Update Player Physics
    custom_player.Movement(delta_time);
    ANIMATION::sprite_update(delta_time);

    // 2. Resolve Collisions with the loaded Map Tiles
    f32 pX = custom_player.Position().x;
    f32 pY = custom_player.Position().y;
    f32 pW = custom_player.Half_Size().x * 2.0f;
    f32 pH = custom_player.Half_Size().y * 2.0f;

    for (auto& tile : map_tiles) {
        f32 oX = tile.pos.x;
        f32 oY = tile.pos.y;
        f32 oW = tile.half_size.x * 2.0f;
        f32 oH = tile.half_size.y * 2.0f;

        // AABB Collision Check
        if (pX - pW / 2 < oX + oW / 2 && pX + pW / 2 > oX - oW / 2 &&
            pY - pH / 2 < oY + oH / 2 && pY + pH / 2 > oY - oH / 2) {

            if (tile.type == 2) {
                // SPIKE COLLISION
                if (damage_timer <= 0.0f) {
                    custom_player.Health() -= 1;
                    damage_timer = 1.0f; // 1 second i-frames
                    camera.Set_Shaking();

                    // Bounce Player Back slightly
                    custom_player.Position().x -= 30.0f;
                    custom_player.Position().y += 20.0f;
                }
            }
            else if (tile.type == 1) {
                // SQUARE COLLISION (Solid Wall/Platform)
                f32 penX = (pW / 2 + oW / 2) - std::abs(pX - oX);
                f32 penY = (pH / 2 + oH / 2) - std::abs(pY - oY);

                if (penX < penY) { // Pushed out horizontally
                    if (pX < oX) custom_player.Position().x -= penX;
                    else custom_player.Position().x += penX;
                }
                else { // Pushed out vertically
                    if (pY < oY) custom_player.Position().y -= penY;
                    else custom_player.Position().y += penY;
                }

                // Update variables for subsequent tile checks
                pX = custom_player.Position().x;
                pY = custom_player.Position().y;
            }
        }
    }

    // 3. Camera Follows Player smoothly
    camera.Position().x = custom_player.Position().x;
    camera.Update();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    // 4. Win/Loss Conditions
    if (custom_player.Health() <= 0) {
        next = GAME_STATE_GAME_OVER;
    }
    else if (custom_player.Position().x > level_end_x + 100.0f && level_end_x > 0.0f) {
        // Player ran past the final tile!
        next = GAME_STATE_VICTORY;
    }
}

void CustomLevel_Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // 1. Draw Background
    ANIMATION::set_sprite_texture(texBackground);
    drawQuad(unit_square, camera.Position().x, camera.Position().y, AEGfxGetWinMaxX() * 2.0f, AEGfxGetWinMaxY() * 2.0f, 1.f, 1.f, 1.f, 1.f);

    // 2. Draw Map Tiles
    for (auto& tile : map_tiles) {
        // Only draw tiles that are relatively close to the camera for performance
        if (std::abs(tile.pos.x - camera.Position().x) < AEGfxGetWinMaxX() + 50.0f) {
            if (tile.type == 1 && texSquare) ANIMATION::set_sprite_texture(texSquare);
            else if (tile.type == 2 && texSpike) ANIMATION::set_sprite_texture(texSpike);
            else ANIMATION::set_sprite_texture(nullptr); // Fallback

            drawQuad(unit_square, tile.pos.x, tile.pos.y, tile.half_size.x * 2.0f, tile.half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    // 3. Draw Player
    if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) { // Flicker when hurt
        ANIMATION::set_sprite_texture(custom_player.Texture());
        drawQuad(custom_player.Mesh(), custom_player.Position().x, custom_player.Position().y,
            custom_player.Half_Size().x * 2.0f, custom_player.Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
    }

    // 4. Draw UI Elements (Health)
    drawHealthBar(unit_square, custom_player, custom_player.Config().MaxHealth());

    // UI Text
    char hp_text[64];
    sprintf_s(hp_text, "HP: %d", custom_player.Health());
    AEGfxPrint(font_id, hp_text, -0.95f, 0.85f, 0.4f, 1.0f, 0.2f, 0.2f, 1.0f);
}

void CustomLevel_Free() {
    AEGfxMeshFree(unit_square);
    map_tiles.clear();
}

void CustomLevel_Unload() {
    AEGfxDestroyFont(font_id);
    if (texSquare) AEGfxTextureUnload(texSquare);
    if (texSpike) AEGfxTextureUnload(texSpike);
    if (texBackground) AEGfxTextureUnload(texBackground);
    AEGfxTextureUnload(custom_player.Texture());
}