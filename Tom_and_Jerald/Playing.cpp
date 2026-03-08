#include "pch.hpp"
#include "utils.hpp"
#include "Playing.hpp"
#include "Obstacle.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Credits.hpp"
#include "Upgrades.hpp"
#include "Camera.hpp"
#include "Animation.hpp"
#include "JetpackFuel.hpp"
#include "Particles.hpp"
#include "Player.hpp"
#include "ImgFontInit.hpp"

// --- FIX: RESTORED MISSING CONSTANTS ---
const f32 k_stage_duration = 120.0f;
const f32 k_damage_cooldown = 1.0f;
const int k_obstacle_count = 10;
const int MAX_ACTIVE_OBSTACLES = 10;

f32 stage_timer = 0.0f;
f32 damage_timer = 0.0f;
JetpackFuel* pFuel = nullptr;

Obstacle obstacles[MAX_ACTIVE_OBSTACLES] = {};

Player base_player;
AEGfxTexture* asteroid_texture = nullptr;
AEGfxTexture* fuel_pickup_texture = nullptr;


namespace {
    Camera camera;
    AEGfxVertexList* unit_square = nullptr;
    AEGfxVertexList* unit_circle = nullptr;

    struct FuelPickup {
        AEVec2 pos;
        bool active;
        f32 size;

        // FIX: Proper initializer list strictly initializes pos to prevent compiler warnings
        FuelPickup() : pos{ 0.0f, 0.0f }, active(false), size(30.0f) {}
    } g_fuel_pickup;

  
}

int getMaxHealthFromUpgrades();

void Playing_Load() {
    ASSETS::Init_Images();
    ASSETS::Init_Font();
}

void Playing_Initialize() {
    createUnitSquare(&unit_square, 0.25f, 0.25f);
    createUnitCircles(&unit_circle);
    createUnitSquare(&(base_player.Mesh()), 0.25f, 0.25f);

    f32 size_reduction = Upgrades_GetSizeReduction();
    f32 upgraded_half_size = 20.0f - size_reduction;
    if (upgraded_half_size < 10.0f) upgraded_half_size = 10.0f;
    AEVec2Set(&(base_player.Position()), 0.0f, 0.0f);
    AEVec2Set(&(base_player.Half_Size()), upgraded_half_size, upgraded_half_size);
    base_player.Health() = getMaxHealthFromUpgrades();

    stage_timer = 0.0f;
    damage_timer = 0.0f;
    Credits_ResetRound();

    f32 base_capacity = 100.0f;
    f32 upgraded_capacity = base_capacity * Upgrades_GetMaxFuelMultiplier();
    pFuel = new JetpackFuel(upgraded_capacity, 30.0f, 2.0f);
    g_fuel_pickup.active = false;

    //Animation______________________________
    //Background
    ANIMATION::background.ImportFromFile("Assets/AnimationData.txt"); //Total rows + columns file located in bin>debuc.assets idk why
    ANIMATION::background.Clip_Select(0, 0, 2, 5.0f);   //Row, start col, frames, fps (BACKGROUND)
    ANIMATION::asteroid.ImportFromFile("Assets/AnimationData.txt"); //Total rows + columns
    ANIMATION::asteroid.Clip_Select(2, 0, 2, 10.0f);    //Row, start col, frames, fps (ASTERIOD)
    ANIMATION::player.ImportFromFile("Assets/AnimationData.txt"); //Total rows + columns
    ANIMATION::player.Clip_Select(0, 0, 2, 10.0f);      //Row, start col, frames, fps (PLAYER)
    //---------------------------------------

    camera.Magnitude() = 20.0f;
    camera.Position().x = base_player.Position().x;
    camera.Position().y = base_player.Position().y;
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    for (int i = 0; i < MAX_ACTIVE_OBSTACLES; ++i) {
        obstacles[i].position.x = randFloat(0.0f, AEGfxGetWinMaxX() * 2.0f);
        obstacles[i].position.y = randFloat(AEGfxGetWinMinY() + 50.0f, AEGfxGetWinMaxY() - 50.0f);
        f32 random_size = randFloat(25.0f, 65.0f);
        obstacles[i].half_size.x = random_size;
        obstacles[i].half_size.y = random_size;
        obstacles[i].velocity.x = randFloat(-80.0f, 80.0f);
        obstacles[i].velocity.y = randFloat(-80.0f, 80.0f);
    }
}

void Playing_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU; return; }

    f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
    bool isFlying = AEInputCheckCurr(AEVK_SPACE) && pFuel->HasFuel();

    if (pFuel) pFuel->Update(delta_time, isFlying);

    //Animation______________________________
    ANIMATION::background.Anim_Update(delta_time);
    ANIMATION::asteroid.Anim_Update(delta_time);
    ANIMATION::player.Anim_Update(delta_time);
    //---------------------------------------

    stage_timer += delta_time;
    if (damage_timer > 0.0f) damage_timer -= delta_time;

    if (pFuel) {
        base_player.Movement(delta_time);
    }

	camera.Follow(base_player.Position());
    camera.Update();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    f32 camX = camera.Position().x;
    // change this to based on player world pos
	f32 offscreen_limit_left{ AEGfxGetWinMinX() - 100.0f };

    for (int i = 0; i < MAX_ACTIVE_OBSTACLES; ++i) {
        obstacles[i].position.x += obstacles[i].velocity.x * delta_time;
        obstacles[i].position.y += obstacles[i].velocity.y * delta_time;

        if (obstacles[i].position.y > AEGfxGetWinMaxY() - 20.0f) obstacles[i].velocity.y *= -1;
        if (obstacles[i].position.y < AEGfxGetWinMinY() + 20.0f) obstacles[i].velocity.y *= -1;

        if (obstacles[i].position.x < offscreen_limit_left) {
            obstacles[i].Reset();
        }
    }

    if (pFuel) {
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
            if (checkOverlap(&base_player.Position(), &base_player.Half_Size(), &g_fuel_pickup.pos, &p_half)) {
                pFuel->RestoreFuel(pFuel->GetMaxFuel() * Upgrades_GetFuelRestorePercentage());
                g_fuel_pickup.active = false;
            }
            if (std::abs(g_fuel_pickup.pos.x - camX) > AEGfxGetWinMaxX() + 500.0f) {
                g_fuel_pickup.active = false;
            }
        }
    }

    bool took_damage = false;
    for (int i = 0; i < MAX_ACTIVE_OBSTACLES; ++i) {
        if (checkOverlap(&(base_player.Position()), &(base_player.Half_Size()), &obstacles[i].position, &obstacles[i].half_size)) {
            if (damage_timer <= 0.0f) {
                base_player.Health() -= 1;
                damage_timer = k_damage_cooldown;
                took_damage = true;
            }
        }
    }

    if (took_damage) {
        camera.Set_Shaking();
        Credits_OnDamage();
        graphics::particleInit(base_player.Position().x, base_player.Position().y, 25);
    }

    bool game_active = (base_player.Health() > 0) && (stage_timer < k_stage_duration);
    Credits_Update(delta_time, game_active);

    if (base_player.Health() <= 0) next = GAME_STATE_GAME_OVER;
    else if (stage_timer >= k_stage_duration) next = GAME_STATE_VICTORY;
    else if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) next = GAME_STATE_QUIT;
}

void Playing_Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

    f32 bg_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    f32 bg_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    f32 cam_x = camera.Position().x;

    f32 offset = fmodf(cam_x, bg_width);
    if (offset < 0) offset += bg_width;

    f32 draw_start_x = cam_x - offset;


    //Animation______________________________
    ANIMATION::background.Anim_Draw(ASSETS::backgroundAssets); //Draws BACKGROUND
    //---------------------------------------
    drawQuad(unit_square, draw_start_x, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);
    AEGfxTextureSet(ASSETS::backgroundAssets, ANIMATION::background.uv_offset_x + 0.25f, ANIMATION::background.uv_offset_y); //Offset by 1 frame
    drawQuad(unit_square, draw_start_x + bg_width, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);

    if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) {

        //Animation______________________________
        ANIMATION::player.Anim_Draw(ASSETS::playerAssets);     //Draws PLAYER
        //_______________________________________

        drawQuad(base_player.Mesh(), base_player.Position().x, base_player.Position().y, base_player.Half_Size().x * 2.0f, base_player.Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
    }

    for (int i = 0; i < MAX_ACTIVE_OBSTACLES; ++i) {

        //Animation______________________________
        ANIMATION::asteroid.Anim_Draw(ASSETS::backgroundAssets);   //Draws ASTEROID
        //---------------------------------------

        Obstacle* obstacle = &obstacles[i];
        drawQuad(unit_square, obstacle->position.x, obstacle->position.y, obstacle->half_size.x * 2.0f, obstacle->half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (g_fuel_pickup.active) {
        if (fuel_pickup_texture) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            //ANIMATION::set_sprite_texture(fuel_pickup_texture);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0.0f, 0.0f);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 0.9f, 0.0f, 1.0f);
        }
    }

    if (pFuel) {
        f32 world_x = base_player.Position().x;
        f32 world_y = base_player.Position().y + base_player.Half_Size().y + 20.0f;
        pFuel->Draw(world_x, world_y, 60.0f, 10.0f);
    }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    drawHealthBar(unit_square, base_player, getMaxHealthFromUpgrades());

    char cheese_text[64];
    sprintf_s(cheese_text, "Cheese: %d", Credits_GetBalance());
    AEGfxPrint(ASSETS::Font(), cheese_text, -0.95f, 0.75f, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);

    char fps_text[64];
    sprintf_s(fps_text, "%.1f FPS", 1 / (f32)AEFrameRateControllerGetFrameTime());
    AEGfxPrint(ASSETS::Font(), fps_text, -0.95f, 0.65f, 0.4f, .0f, .0f, .0f, 1.0f);

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    char timer_text[64];
    f32 time_left = k_stage_duration - stage_timer;
    if (time_left < 0.0f) time_left = 0.0f;
    sprintf_s(timer_text, "TIME LEFT: %.1f", time_left);
    AEGfxPrint(ASSETS::Font(), timer_text, -0.95f, 0.85f, 0.45f, 0.9f, 0.9f, 0.9f, 1.0f);

    graphics::particleDraw(unit_circle);
}

void Playing_Free() {
    AEGfxMeshFree(unit_square);
    AEGfxMeshFree(unit_circle);
    if (pFuel) { delete pFuel; pFuel = nullptr; }
}

void Playing_Unload() {
    ASSETS::Unload_Images();
    ASSETS::Unload_Font();

}

int getMaxHealthFromUpgrades() {
    f32 increase = Upgrades_GetHealthIncrease();
    f32 multiplier = 1.0f + increase;
    int max_health = (int)floorf(base_player.Config().MaxHealth() * multiplier);
    if (max_health < 1) max_health = 1;
    return max_health;
}