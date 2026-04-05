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
#include "ImgFontInit.hpp"
#include "LevelEditor.hpp"
#include "Audio.hpp"

namespace {
    Player* custom_player;

    int select_level{};

    std::vector<LevelTile> map_tiles{};
    s8 font_id = -1;
    const int MAX_ACTIVE_OBSTACLES = 10;
    ObstacleSystem obstacle_system;
    Camera camera;
    JetpackFuel* pFuel = nullptr;

    AEGfxVertexList* unit_square = nullptr;

    f32 damage_timer = 0.0f;
    f32 level_end_x = 1000.0f;

    // Hot-reload: track which file is loaded and its last write time
    std::string  g_level_filename;
    std::filesystem::file_time_type g_level_last_write;
    f32          g_reload_check_timer = 0.0f;
    bool         g_reload_flash       = false;
    f32          g_reload_flash_timer = 0.0f;

    struct FuelPickup {
        AEVec2 pos;
        bool active;
        f32 size;

        FuelPickup() : pos{ 0.0f, 0.0f }, active(false), size(30.0f) {}
    } g_fuel_pickup;

    f32 g_fuel_spawn_timer = 0.0f;
    f32 g_fuel_spawn_interval = 15.0f;

    // Gameplay constants — loaded from Assets/data/GameConfig.txt in CustomLevel_Load
    f32 k_cl_player_base_half_size = 20.0f;
    f32 k_cl_player_min_half_size  = 10.0f;
    f32 k_cl_fuel_base_capacity    = 100.0f;
    f32 k_cl_fuel_burn_time        = 30.0f;
    f32 k_cl_fuel_passive_drain    = 1.0f;
    f32 k_cl_fuel_spawn_min        = 10.0f;
    f32 k_cl_fuel_spawn_max        = 20.0f;
    f32 k_cl_camera_magnitude      = 20.0f;
    bool g_debug_mode              = false;  // toggled with key 6
}

void CustomLevel_Load() {
    ASSETS::Init_Images();
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	/*ASSETS::Init_Font();*/

    // Load designer-editable constants from config file
    auto cfg = LoadConfig("Assets/data/GameConfig.txt");
    k_cl_player_base_half_size = ConfigFloat(cfg, "custom_player_base_half_size", k_cl_player_base_half_size);
    k_cl_player_min_half_size  = ConfigFloat(cfg, "custom_player_min_half_size",  k_cl_player_min_half_size);
    k_cl_fuel_base_capacity    = ConfigFloat(cfg, "fuel_base_capacity",           k_cl_fuel_base_capacity);
    k_cl_fuel_burn_time        = ConfigFloat(cfg, "fuel_burn_time",               k_cl_fuel_burn_time);
    k_cl_fuel_passive_drain    = ConfigFloat(cfg, "fuel_passive_drain",           k_cl_fuel_passive_drain);
    k_cl_fuel_spawn_min        = ConfigFloat(cfg, "fuel_spawn_min",               k_cl_fuel_spawn_min);
    k_cl_fuel_spawn_max        = ConfigFloat(cfg, "fuel_spawn_max",               k_cl_fuel_spawn_max);
    k_cl_camera_magnitude      = ConfigFloat(cfg, "camera_magnitude",             k_cl_camera_magnitude);
}

void CustomLevel_Initialize() {
	custom_player = new Player();
    createUnitSquare(&unit_square, 0.25f, 0.25f);
    createUnitSquare(&(*custom_player).Mesh(), 0.25f, 0.25f);
    //Animation______________________________
    ANIMATION::background.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::background.Clip_Select(0, 0, 3, 5.0f);
    ANIMATION::player.ImportFromFile("Assets/AnimationData.txt"); //Total rows + columns
    ANIMATION::player.Clip_Select(0, 0, 3, 10.0f);
    ANIMATION::asteroid.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::asteroid.Clip_Select(2, 0, 3, 10.0f);
    ANIMATION::spike.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::spike.Clip_Select(1, 1, 1, 10.0f);
    //---------------------------------------
    damage_timer = 0.0f;

    f32 size_reduction = Upgrades_GetSizeReduction();
    f32 upg_size = k_cl_player_base_half_size - size_reduction;
    if (upg_size < k_cl_player_min_half_size) upg_size = k_cl_player_min_half_size;

    AEVec2Set(&(*custom_player).Position(), 0.0f, 0.0f);
    AEVec2Set(&(*custom_player).Half_Size(), upg_size, upg_size);

    int max_health = (int)std::floor((*custom_player).Config().MaxHealth() * (1.0f + Upgrades_GetHealthIncrease()));
    (*custom_player).Health() = max_health;

    camera.Magnitude() = k_cl_camera_magnitude;
	camera.Position() = (*custom_player).Position();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    f32 upg_fuel = k_cl_fuel_base_capacity * Upgrades_GetMaxFuelMultiplier();
    f32 burn_rate = (upg_fuel / k_cl_fuel_burn_time) - k_cl_fuel_passive_drain;
    pFuel = new JetpackFuel(upg_fuel, burn_rate, k_cl_fuel_passive_drain);
    g_fuel_pickup.active = false;
    g_fuel_spawn_timer = 0.0f;
    g_fuel_spawn_interval = randFloat(k_cl_fuel_spawn_min, k_cl_fuel_spawn_max);


    // Get level to load from this file
    std::ifstream inFile("MapLevel/LoadLevel.txt");
    inFile >> select_level;
    inFile.close();

    g_level_filename = "MapLevel/ExportedLevel" + std::to_string(select_level) + ".txt";

    // Load level data from file, populating map_tiles and obstacle_system
    LoadLevelDataFromFile(g_level_filename, level_end_x, map_tiles, obstacle_system);

    // Snapshot last-write time for hot-reload detection
    g_reload_check_timer = 0.0f;
    g_reload_flash       = false;
    g_reload_flash_timer = 0.0f;
    if (std::filesystem::exists(g_level_filename))
        g_level_last_write = std::filesystem::last_write_time(g_level_filename);

}

void CustomLevel_Update() {
    f32 dt = (f32)AEFrameRateControllerGetFrameTime();
    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU; return; }
    if (AEInputCheckTriggered(AEVK_6)) g_debug_mode = !g_debug_mode;  // toggle debug overlay
    if (damage_timer > 0.0f) damage_timer -= dt;

    bool isFlying = (AEInputCheckCurr(AEVK_SPACE) || AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_UP)) && pFuel->HasFuel();
    pFuel->Update(dt, isFlying);

    (*custom_player).Movement(dt, isFlying);
    //Animation______________________________
    ANIMATION::background.Anim_Update(dt);
    ANIMATION::player.Anim_Update(dt);
    ANIMATION::asteroid.Anim_Update(dt);
    //---------------------------------------

    // Hot-reload: check every second whether the level file changed on disk
    g_reload_check_timer += dt;
    if (g_reload_check_timer >= 1.0f) {
        g_reload_check_timer = 0.0f;
        if (std::filesystem::exists(g_level_filename)) {
            auto new_time = std::filesystem::last_write_time(g_level_filename);
            if (new_time != g_level_last_write) {
                g_level_last_write = new_time;
                map_tiles.clear();
                obstacle_system.Obstacles().clear();
                level_end_x = 1000.0f;
                LoadLevelDataFromFile(g_level_filename, level_end_x, map_tiles, obstacle_system);
                g_reload_flash       = true;
                g_reload_flash_timer = 2.0f;
            }
        }
    }
    if (g_reload_flash_timer > 0.0f) g_reload_flash_timer -= dt;
    else                               g_reload_flash = false;

	// Camera follows player, but update also update to ensure shake if toggle is set
    camera.Follow((*custom_player).Position());
    camera.Update();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);


    f32 camX = camera.Position().x;
    f32 offscreen_limit = AEGfxGetWinMaxX() + 150.0f;

    bool took_damage = false;

    auto& obstacles = obstacle_system.Obstacles();
    for (auto iter = obstacles.begin(); iter != obstacles.end(); ) {
		iter->Update(dt, camX, offscreen_limit, false);


        if (checkOverlap(( &(*custom_player).Position()), &(*custom_player).Half_Size(), iter->PositionPtr(), iter->HalfSizePtr())) {
            if (damage_timer <= 0.0f) {
                ratsqueakAudio.Play();
                (*custom_player).Health() -= 1;
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

    // Timer-based fuel pickup spawn: every 10-20 seconds
    if (!g_fuel_pickup.active) {
        g_fuel_spawn_timer += dt;
        if (g_fuel_spawn_timer >= g_fuel_spawn_interval) {
            f32 spawnOffset = AEGfxGetWinMaxX() + 200.0f;
            if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
                spawnOffset = -AEGfxGetWinMaxX() - 200.0f;
            }
            g_fuel_pickup.pos.x = camX + spawnOffset;
            g_fuel_pickup.pos.y = randFloat(AEGfxGetWinMinY() + 50.0f, AEGfxGetWinMaxY() - 50.0f);
            g_fuel_pickup.active = true;
            g_fuel_spawn_timer = 0.0f;
            g_fuel_spawn_interval = randFloat(k_cl_fuel_spawn_min, k_cl_fuel_spawn_max);
        }
    }

    if (g_fuel_pickup.active) {
        AEVec2 p_half = { g_fuel_pickup.size / 2.0f, g_fuel_pickup.size / 2.0f };
        if (checkOverlap(&(*custom_player).Position(), &(*custom_player).Half_Size(), &g_fuel_pickup.pos, &p_half)) {
            pFuel->RestoreFuel(pFuel->GetMaxFuel() * Upgrades_GetFuelRestorePercentage());
            g_fuel_pickup.active = false;
            g_fuel_spawn_timer = 0.0f;
            g_fuel_spawn_interval = randFloat(k_cl_fuel_spawn_min, k_cl_fuel_spawn_max);
        }
        if (std::abs(g_fuel_pickup.pos.x - camX) > AEGfxGetWinMaxX() + 500.0f) {
            g_fuel_pickup.active = false;
        }
    }

    // removed for now
    /*f32 pX = custom_player.Position().x, pY = custom_player.Position().y;
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
    }*/

    if ((*custom_player).Health() <= 0) {
        next = GAME_STATE_GAME_OVER;
    }
    else if ((*custom_player).Position().x > level_end_x + 100.0f && level_end_x > 0.0f) {
        next = GAME_STATE_VICTORY;
    }
}

void CustomLevel_Draw() {

	// Clear the screen & Set to Default
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    /******************************************************************************/


    // Draw Background Block
    //Animation______________________________
    ANIMATION::background.Anim_Draw(ASSETS::backgroundAssets);
    //---------------------------------------
    f32 bg_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    f32 bg_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    f32 cam_x = camera.Position().x;
    f32 offset = fmodf(cam_x, bg_width);
    if (offset < 0) offset += bg_width;
    f32 draw_start_x = cam_x - offset;
    drawQuad(unit_square, draw_start_x, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);
    drawQuad(unit_square, draw_start_x + bg_width, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);
    /******************************************************************************/
    
	// Drawing of the obstacles
    auto& obstacles = obstacle_system.Obstacles();
    for (auto iter = obstacles.begin(); iter != obstacles.end(); ++iter) {
        switch (iter->Type()) {
        case ObstacleType::Asteroid:
            //Animation______________________________
            ANIMATION::asteroid.Anim_Draw(ASSETS::backgroundAssets);
            //---------------------------------------
            break;
        case ObstacleType::Spike:
            //Animation______________________________
            ANIMATION::spike.Anim_Draw(ASSETS::backgroundAssets);
            //---------------------------------------
            break;
        }
        drawQuad(unit_square, iter->position.x, iter->position.y, iter->half_size.x * 2.0f, iter->half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    /******************************************************************************/

	// Draw Player, with flashing effect when taking damage
    if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) {
        //Animation______________________________
        ANIMATION::player.Anim_Draw(ASSETS::playerAssets);
        //---------------------------------------
        drawQuad((*custom_player).Mesh(), (*custom_player).Position().x, (*custom_player).Position().y, (*custom_player).Half_Size().x * 2.0f, (*custom_player).Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
    }
    /******************************************************************************/

	// Draw Fuel Pickup
    if (g_fuel_pickup.active) {
        if (ASSETS::otherAssets) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            //ANIMATION::set_sprite_texture(ASSETS::otherAssets);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0.0f, 0.0f);
            drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 0.9f, 0.0f, 1.0f);
        }
    }
    /******************************************************************************/
	
    // Draw Fuel Bar
    if (pFuel) {
        f32 world_x = (*custom_player).Position().x;
        f32 world_y = (*custom_player).Position().y + (*custom_player).Half_Size().y + 20.0f;
        pFuel->Draw(world_x, world_y, 60.0f, 10.0f);
    }
    /******************************************************************************/

	// Draw Health Bar
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    drawHealthBar(unit_square, (*custom_player), (int)std::floor((*custom_player).Config().MaxHealth() * (1.0f + Upgrades_GetHealthIncrease())));
    /******************************************************************************/
 
	// Draw HP Text
    char hp_text[64];
    sprintf_s(hp_text, "HP: %d", (*custom_player).Health());
    AEGfxPrint(ASSETS::Font(), hp_text, -0.95f, 0.85f, 0.4f, 1.0f, 0.2f, 0.2f, 1.0f);
    /******************************************************************************/

    // Hot-reload flash notification
    if (g_reload_flash)
        AEGfxPrint(ASSETS::Font(), "LEVEL RELOADED", -0.35f, 0.0f, 0.8f, 0.2f, 1.0f, 0.3f, 1.0f);

    // ---- DEBUG OVERLAY — press 6 to toggle ----
    if (g_debug_mode) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxTextureSet(NULL, 0.0f, 0.0f);

        // Player bounding box — green
        drawQuad(unit_square,
                 (*custom_player).Position().x, (*custom_player).Position().y,
                 (*custom_player).Half_Size().x * 2.0f, (*custom_player).Half_Size().y * 2.0f,
                 0.0f, 1.0f, 0.0f, 0.4f);

        // Obstacle bounding boxes + asteroid speed readout
        int ast_idx = 0;
        char dbg_buf[128];
        auto& dbg_obs = obstacle_system.Obstacles();
        for (Obstacle const& obs : dbg_obs) {
            switch (obs.type) {
            case Asteroid:
                drawQuad(unit_square, obs.position.x, obs.position.y,
                         obs.half_size.x * 2.0f, obs.half_size.y * 2.0f,
                         1.0f, 0.2f, 0.2f, 0.4f);
                sprintf_s(dbg_buf, "AST[%d] vx:%.0f vy:%.0f", ast_idx,
                          obs.velocity.x, obs.velocity.y);
                AEGfxPrint(font_id, dbg_buf,
                           -0.95f, 0.50f - ast_idx * 0.09f,
                           0.45f, 1.0f, 0.5f, 0.5f, 1.0f);
                ++ast_idx;
                break;
            case Spike:
                drawQuad(unit_square, obs.position.x, obs.position.y,
                         obs.half_size.x * 2.0f, obs.half_size.y * 2.0f,
                         1.0f, 1.0f, 0.0f, 0.4f, obs.rotation);
                break;
            case Wall:
                drawQuad(unit_square, obs.position.x, obs.position.y,
                         obs.half_size.x * 2.0f, obs.half_size.y * 2.0f,
                         1.0f, 0.5f, 0.0f, 0.4f, obs.rotation);
                break;
            default: break;
            }
        }

        // Legend
        AEGfxPrint(font_id,
                   "[DEBUG 6:off] GREEN=player  RED=asteroid  YELLOW=spike  ORANGE=wall",
                   -0.95f, 0.95f, 0.33f, 0.2f, 1.0f, 0.2f, 1.0f);
        if (ast_idx == 0)
            AEGfxPrint(font_id, "No asteroids active",
                       -0.95f, 0.50f, 0.45f, 0.7f, 0.7f, 0.7f, 1.0f);
    }
    // ---- END DEBUG OVERLAY ----
}

void CustomLevel_Free() {
    if (unit_square) { AEGfxMeshFree(unit_square); unit_square = nullptr; }
    
    
	// Free map tile vector memory by swapping with an empty vector
    map_tiles.clear();
    obstacle_system.Obstacles().clear();
    // lets try this for now
    std::vector<LevelTile>().swap(map_tiles);
    std::vector<Obstacle>().swap(obstacle_system.Obstacles());

	map_tiles.shrink_to_fit();
	obstacle_system.Obstacles().shrink_to_fit();

    if (pFuel) { delete pFuel; pFuel = nullptr; }
    
    delete custom_player;
    custom_player = nullptr;

}

void CustomLevel_Unload() {
	AEGfxDestroyFont(font_id);
    ASSETS::Unload_Images();
	/*ASSETS::Unload_Font();*/
}