#include "pch.hpp"
#include "CustomLevel.hpp"
#include "Player.hpp"
#include "Obstacle.hpp"
#include "Camera.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "utils.hpp"
#include "Animation.hpp"
#include "JetpackFuel.hpp"
#include "Upgrades.hpp"
#include "Credits.hpp"
#include "LevelTile.hpp"

Player custom_player;
namespace {
    std::vector<LevelTile> map_tiles;

    const int MAX_ACTIVE_OBSTACLES = 10;
    ObstacleSystem obstacle_system(0);

    Camera camera;
    JetpackFuel* pFuel = nullptr;

    AEGfxTexture* texSquare = nullptr;
    AEGfxTexture* texSpike = nullptr;
    AEGfxTexture* texBackground = nullptr;
    AEGfxTexture* fuel_pickup_texture = nullptr;
    AEGfxTexture* asteroid_texture = nullptr;
    AEGfxVertexList* unit_square = nullptr;
    s8 font_id;

    f32 damage_timer = 0.0f;
    f32 level_end_x = 1000.0f;

    struct FuelPickup {
        AEVec2 pos;
        bool active;
        f32 size;

        FuelPickup() : pos{ 0.0f, 0.0f }, active(false), size(30.0f) {}
    } g_fuel_pickup;

}

void CustomLevel_Load() {
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
    texSquare = AEGfxTextureLoad("Assets/Square.png");
    texSpike = AEGfxTextureLoad("Assets/Spike.png");
    texBackground = AEGfxTextureLoad("Assets/Game_Background.png");
    fuel_pickup_texture = AEGfxTextureLoad("Assets/FuelPickup.png");
    asteroid_texture = AEGfxTextureLoad("Assets/PlanetTexture.png");
    custom_player.Texture() = AEGfxTextureLoad("Assets/Fairy_Rat.png");
}

void CustomLevel_Initialize() {
    createUnitSquare(&unit_square, 0.5f, 0.5f);
    createUnitSquare(&(custom_player.Mesh()), 0.5f, 0.5f);
    ANIMATION::sprite_Initialize();

    map_tiles.clear();
    damage_timer = 0.0f;

    f32 size_reduction = Upgrades_GetSizeReduction();
    f32 upg_size = 20.0f - size_reduction;
    if (upg_size < 10.0f) upg_size = 10.0f;

    AEVec2Set(&custom_player.Position(), 0.0f, 0.0f);
    AEVec2Set(&custom_player.Half_Size(), upg_size, upg_size);

    int max_health = (int)std::floor(custom_player.Config().MaxHealth() * (1.0f + Upgrades_GetHealthIncrease()));
    custom_player.Health() = max_health;

    camera.Magnitude() = 20.0f;
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    f32 upg_fuel = 100.0f * Upgrades_GetMaxFuelMultiplier();
    pFuel = new JetpackFuel(upg_fuel, 30.0f, 2.0f);
    g_fuel_pickup.active = false;

	LoadLevelDataFromFile("ExportedLevel.txt", level_end_x, map_tiles, obstacle_system);
}

void CustomLevel_Update() {
    f32 dt = (f32)AEFrameRateControllerGetFrameTime();
    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU; return; }
    if (damage_timer > 0.0f) damage_timer -= dt;

    bool isFlying = AEInputCheckCurr(AEVK_SPACE) && pFuel->HasFuel();
    pFuel->Update(dt, isFlying);

    custom_player.Movement(dt);
    ANIMATION::sprite_update(dt);

    camera.Position().x = custom_player.Position().x;
    camera.Update();
	camera.Follow(custom_player.Position());
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    f32 camX = camera.Position().x;
    f32 offscreen_limit = AEGfxGetWinMaxX() + 150.0f;

    bool took_damage = false;

    auto& obstacles = obstacle_system.Obstacles();
    for (auto iter = obstacles.begin(); iter != obstacles.end(); ) {
		iter->Update(dt, camX, offscreen_limit, false);


        if (checkOverlap(&(custom_player.Position()), &(custom_player.Half_Size()), iter->PositionPtr(), iter->HalfSizePtr())) {
            if (damage_timer <= 0.0f) {
                custom_player.Health() -= 1;
                damage_timer = 1.0f;
                took_damage = true;
            }
        }

        // if type is non obstacle we need to pop it
        if (iter->Type() == Non_Obstacle) {
            iter = obstacle_system.Obstacles().erase(iter);
        }
        else {
            ++iter;
        }
    }

    if (took_damage) {
        camera.Set_Shaking();
        Credits_OnDamage();
    }

    f32 fuel_ratio = pFuel->GetCurrentFuel() / pFuel->GetMaxFuel();
    if (!g_fuel_pickup.active && fuel_ratio <= 0.50f) {
        f32 spawnOffset = AEGfxGetWinMaxX() + 200.0f;
        if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
            spawnOffset = -AEGfxGetWinMaxX() - 200.0f;
        }
        g_fuel_pickup.pos.x = camX + spawnOffset;
        g_fuel_pickup.pos.y = randFloat(AEGfxGetWinMinY() + 50.0f, AEGfxGetWinMaxY() - 50.0f);
        g_fuel_pickup.active = true;
    }

    if (g_fuel_pickup.active) {
        AEVec2 p_half = { g_fuel_pickup.size / 2.0f, g_fuel_pickup.size / 2.0f };
        if (checkOverlap(&custom_player.Position(), &custom_player.Half_Size(), &g_fuel_pickup.pos, &p_half)) {
            pFuel->RestoreFuel(pFuel->GetMaxFuel() * Upgrades_GetFuelRestorePercentage());
            g_fuel_pickup.active = false;
        }
        if (std::abs(g_fuel_pickup.pos.x - camX) > AEGfxGetWinMaxX() + 500.0f) {
            g_fuel_pickup.active = false;
        }
    }

    // what does this do
    f32 pX = custom_player.Position().x, pY = custom_player.Position().y;
    f32 pW = custom_player.Half_Size().x * 2.0f, pH = custom_player.Half_Size().y * 2.0f;

    for (auto& tile : map_tiles) {
        f32 oX = tile.pos.x, oY = tile.pos.y;
        f32 oW = tile.half_size.x * 2.0f, oH = tile.half_size.y * 2.0f;

        if (pX - pW / 2 < oX + oW / 2 && pX + pW / 2 > oX - oW / 2 && pY - pH / 2 < oY + oH / 2 && pY + pH / 2 > oY - oH / 2) {
            if (tile.type == 2) {
                if (damage_timer <= 0.0f) {
                    custom_player.Health() -= 1;
                    damage_timer = 1.0f;
                    took_damage = true;
                    custom_player.Position().x -= 30.0f;
                    custom_player.Position().y += 20.0f;
                }
            }
            else if (tile.type == 1) {
                f32 penX = (pW / 2 + oW / 2) - std::abs(pX - oX);
                f32 penY = (pH / 2 + oH / 2) - std::abs(pY - oY);
                if (penX < penY) {
                    if (pX < oX) custom_player.Position().x -= penX;
                    else custom_player.Position().x += penX;
                }
                else {
                    if (pY < oY) custom_player.Position().y -= penY;
                    else custom_player.Position().y += penY;
                }
                pX = custom_player.Position().x; pY = custom_player.Position().y;
            }
        }
    }

    if (took_damage) camera.Set_Shaking();

    if (custom_player.Health() <= 0) {
        next = GAME_STATE_GAME_OVER;
    }
    else if (custom_player.Position().x > level_end_x + 100.0f && level_end_x > 0.0f) {
        next = GAME_STATE_VICTORY;
    }
}

void CustomLevel_Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    ANIMATION::set_sprite_texture(texBackground);
    f32 bg_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    f32 bg_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    f32 cam_x = camera.Position().x;

    f32 offset = fmodf(cam_x, bg_width);
    if (offset < 0) offset += bg_width;
    f32 draw_start_x = cam_x - offset;
	//Draws Background
    drawQuad(unit_square, cam_x, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);
    
    // Not needed i think
    /*for (auto& tile : map_tiles) {
        if (std::abs(tile.pos.x - camera.Position().x) < AEGfxGetWinMaxX() + 50.0f) {
            if (tile.type == 1 && texSquare) ANIMATION::set_sprite_texture(texSquare);
            else if (tile.type == 2 && texSpike) ANIMATION::set_sprite_texture(texSpike);
            else ANIMATION::set_sprite_texture(nullptr);
            drawQuad(unit_square, tile.pos.x, tile.pos.y, tile.half_size.x * 2.0f, tile.half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }*/

	// Drawing of the obstacles
    ANIMATION::set_sprite_texture(asteroid_texture);
    
    auto& obstacles = obstacle_system.Obstacles();
    for (auto iter = obstacles.begin(); iter != obstacles.end(); ++iter) {
        switch (iter->Type()) {
        case ObstacleType::Asteroid:
            ANIMATION::set_sprite_texture(asteroid_texture);
            break;
        case ObstacleType::Spike:
            ANIMATION::set_sprite_texture(texSpike);
            break;
        }
        drawQuad(unit_square, iter->position.x, iter->position.y, iter->half_size.x * 2.0f, iter->half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    }

    if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) {
        ANIMATION::set_sprite_texture(custom_player.Texture());
        drawQuad(custom_player.Mesh(), custom_player.Position().x, custom_player.Position().y, custom_player.Half_Size().x * 2.0f, custom_player.Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
    }

    if (g_fuel_pickup.active) {
        if (fuel_pickup_texture) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            ANIMATION::set_sprite_texture(fuel_pickup_texture);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0.0f, 0.0f);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 0.9f, 0.0f, 1.0f);
        }
    }

    if (pFuel) {
        f32 world_x = custom_player.Position().x;
        f32 world_y = custom_player.Position().y + custom_player.Half_Size().y + 20.0f;
        pFuel->Draw(world_x, world_y, 60.0f, 10.0f);
    }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    drawHealthBar(unit_square, custom_player, (int)std::floor(custom_player.Config().MaxHealth() * (1.0f + Upgrades_GetHealthIncrease())));

    char hp_text[64];
    sprintf_s(hp_text, "HP: %d", custom_player.Health());
    AEGfxPrint(font_id, hp_text, -0.95f, 0.85f, 0.4f, 1.0f, 0.2f, 0.2f, 1.0f);
}

void CustomLevel_Free() {
    AEGfxMeshFree(unit_square);
    map_tiles.clear();
    if (pFuel) { delete pFuel; pFuel = nullptr; }
}

void CustomLevel_Unload() {
    AEGfxDestroyFont(font_id);
    if (texSquare) AEGfxTextureUnload(texSquare);
    if (texSpike) AEGfxTextureUnload(texSpike);
    if (texBackground) AEGfxTextureUnload(texBackground);
    if (fuel_pickup_texture) AEGfxTextureUnload(fuel_pickup_texture);
    if (asteroid_texture) AEGfxTextureUnload(asteroid_texture);
}