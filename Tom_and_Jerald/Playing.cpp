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
#include "Audio.hpp"
#include "DifferentGamemode.hpp"
#include "Enemy.hpp"

namespace {

    int gTotalScore = 0;

    // --- FIX: RESTORED MISSING CONSTANTS ---
    const f32 k_stage_duration = 120.0f;
    const f32 k_damage_cooldown = 1.0f;
    const int k_obstacle_count = 10;
    const int MAX_ACTIVE_OBSTACLES = 10;
    const f32 k_obstacle_spawn_cooldown = 3.0f;


    // Camera
    Camera camera;
    // Meshes
    AEGfxVertexList* unit_square = nullptr;
    AEGfxVertexList* unit_circle = nullptr;
    AEGfxVertexList* bgMesh = nullptr;
    // Fuel
    struct FuelPickup {
        AEVec2 pos;
        bool active;
        f32 size;

        // FIX: Proper initializer list strictly initializes pos to prevent compiler warnings
        FuelPickup() : pos{ 0.0f, 0.0f }, active(false), size(30.0f) {}
    } g_fuel_pickup;
    // Portal
    namespace {
        struct PortalData
        {
            AEVec2 pos{};
            AEVec2 half_size{ 30.0f, 60.0f };
            bool active = false;
        };

        struct SavedPlayingState
        {
            bool valid = false;
            AEVec2 player_pos{};
            AEVec2 player_vel{};
            AEVec2 player_half_size{};
            int player_health = 0;

            f32 stage_timer = 0.0f;
            f32 damage_timer = 0.0f;
            f32 obstacle_last_spawn_time = 0.0f;

            bool fuel_pickup_active = false;
            decltype(g_fuel_pickup) fuel_pickup{};

            std::vector<Obstacle> obstacles;
            Cat saved_cat{};
        };

        PortalData gMazePortal;
        SavedPlayingState gSavedPlayingState;
        bool gResumeFromMaze = false;

        f32 gPortalRespawnTimer = 0.0f;
        const f32 kPortalRespawnDelay = 5.0f;
        bool gPortalWaitingToRespawn = false;
    }


    // Enemy Cat
    Cat enemy_cat;

    s8 font_id = -1;
    BOOL quitting_flag = FALSE;

    f32 stage_timer = 0.0f;
    f32 damage_timer = 0.0f;
    JetpackFuel* pFuel = nullptr;

    std::vector<Obstacle> obstacles;
    f32 obstacle_last_spawn_time = 0.0f;
    //Obstacle obstacles[MAX_ACTIVE_OBSTACLES] = {};

    Player base_player;
    AEGfxTexture* asteroid_texture = nullptr;
    AEGfxTexture* fuel_pickup_texture = nullptr;

    // check if the game is paused
    bool isPaused;

    // Fuel pickup spawn timer (10-20 second random interval)
    f32 g_fuel_spawn_timer = 0.0f;
    f32 g_fuel_spawn_interval = 15.0f;


}

int getMaxHealthFromUpgrades();

void Playing_Load() {
    ASSETS::Init_Images();
    ASSETS::Init_Font();
    font_id = ASSETS::Font();
}

static void SavePlayingStateForMaze()
{
    gSavedPlayingState.valid = true;
    gSavedPlayingState.player_pos = base_player.Position();
    gSavedPlayingState.player_vel = base_player.Velocity();
    gSavedPlayingState.player_half_size = base_player.Half_Size();
    gSavedPlayingState.player_health = base_player.Health();

    gSavedPlayingState.stage_timer = stage_timer;
    gSavedPlayingState.damage_timer = damage_timer;
    gSavedPlayingState.obstacle_last_spawn_time = obstacle_last_spawn_time;

    gSavedPlayingState.fuel_pickup_active = g_fuel_pickup.active;
    gSavedPlayingState.fuel_pickup = g_fuel_pickup;

    gSavedPlayingState.obstacles = obstacles;

    gSavedPlayingState.saved_cat = enemy_cat;
}

static void RestorePlayingStateFromMaze()
{
    if (!gSavedPlayingState.valid)
        return;

    base_player.Position() = gSavedPlayingState.player_pos;
    base_player.Velocity() = gSavedPlayingState.player_vel;
    base_player.Half_Size() = gSavedPlayingState.player_half_size;
    base_player.Health() = gSavedPlayingState.player_health;

    stage_timer = gSavedPlayingState.stage_timer;
    damage_timer = gSavedPlayingState.damage_timer;
    obstacle_last_spawn_time = gSavedPlayingState.obstacle_last_spawn_time;

    g_fuel_pickup = gSavedPlayingState.fuel_pickup;
    g_fuel_pickup.active = gSavedPlayingState.fuel_pickup_active;

    obstacles = gSavedPlayingState.obstacles;

    camera.Position().x = base_player.Position().x;
    camera.Position().y = base_player.Position().y;
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    enemy_cat = gSavedPlayingState.saved_cat;
}

static void ApplyMazeOutcome()
{
    if (!gMazeCompleted)
        return;

    if (gMazeSuccess)
    {
        gTotalScore += 1000;
        Credits_Add(300);
        base_player.Health() = getMaxHealthFromUpgrades();
    }
    else
    {
        int new_health = (base_player.Health() + 1) / 2;
        base_player.Health() = new_health;
    }

    gMazeCompleted = false;
    gMazeSuccess = false;
    gMazeFromPlaying = false;
    gSavedPlayingState.valid = false;

    if (base_player.Health() <= 0)
        next = GAME_STATE_GAME_OVER;
}

static void SpawnPortalAhead()
{
    gMazePortal.active = true;
    gPortalWaitingToRespawn = false;
    gPortalRespawnTimer = 0.0f;

    f32 screenWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();

    gMazePortal.pos.x = camera.Position().x + screenWidth + 300.0f;
    gMazePortal.pos.y = randFloat(AEGfxGetWinMinY() + 80.0f, AEGfxGetWinMaxY() - 80.0f);
}

void Playing_Initialize() {
	quitting_flag = FALSE;
    
    createUnitSquare(&unit_square, 0.25f, 0.25f);
    createUnitCircles(&unit_circle);
    createUnitSquare(&(base_player.Mesh()), 0.25f, 0.25f);

    createUnitSquare(&bgMesh, 0.25f, 0.25f, 1.0);

    // Changing Player size
    f32 size_reduction = Upgrades_GetSizeReduction();
    f32 upgraded_half_size = 50.0f - size_reduction;
    if (upgraded_half_size < 10.0f) upgraded_half_size = 10.0f;
    AEVec2Set(&(base_player.Position()), 0.0f, 0.0f);
    AEVec2Set(&(base_player.Half_Size()), upgraded_half_size, upgraded_half_size);
    // ==================================
    base_player.Health() = getMaxHealthFromUpgrades();

    stage_timer = 0.0f;
    damage_timer = 0.0f;
    Credits_LoadFile("Assets/data/Cheese.txt");
    Credits_ResetRound();

    f32 base_capacity = 100.0f;
    f32 upgraded_capacity = base_capacity * Upgrades_GetMaxFuelMultiplier();
    // Drain rate: at full fuel, continuous thrust drains fuel in exactly 30 seconds.
    // total_drain = passiveDrain + burnRate = upgraded_capacity / 30
    const f32 k_passive_drain = 1.0f;
    f32 burn_rate = (upgraded_capacity / 30.0f) - k_passive_drain;
    pFuel = new JetpackFuel(upgraded_capacity, burn_rate, k_passive_drain);
    g_fuel_pickup.active = false;
    g_fuel_spawn_timer = 0.0f;
    g_fuel_spawn_interval = randFloat(10.0f, 20.0f);

    if (gResumeFromMaze && gSavedPlayingState.valid)
    {
        RestorePlayingStateFromMaze();
        ApplyMazeOutcome();
        gResumeFromMaze = false;
        return;
    }
    gTotalScore = 0;


    //Animation______________________________
    //Background
    ANIMATION::background.ImportFromFile("Assets/AnimationData.txt"); //Total rows + columns file located in bin>debuc.assets idk why
    ANIMATION::background.Clip_Select(0, 0, 4, 1.0f);                 //Row, start col, frames, fps (BACKGROUND)
    ANIMATION::asteroid.ImportFromFile("Assets/AnimationData.txt");   //Total rows + columns
    ANIMATION::asteroid.Clip_Select(2, 0, 2, 10.0f);                  //Row, start col, frames, fps (ASTERIOD)
    ANIMATION::player.ImportFromFile("Assets/AnimationData.txt");     //Total rows + columns
    ANIMATION::player.Clip_Select(0, 0, 2, 10.0f);                    //Row, start col, frames, fps (PLAYER)

    ANIMATION::spike.ImportFromFile("Assets/AnimationData.txt");      //Total rows + columns
    // Spike at 1,1 of spritesheet with only 1 sprite
    ANIMATION::spike.Clip_Select(1, 1, 1, 10.0f);                     //Row, start col, frames, fps (Spike)
    
    ANIMATION::wall.ImportFromFile("Assets/AnimationData.txt");       //Total rows + columns
    ANIMATION::wall.Clip_Select(1, 0, 1, 10.0f);                      //Row, start col, frames, fps (Wall)

	ANIMATION::cat.ImportFromFile("Assets/AnimationData.txt");        //Total rows + columns
	ANIMATION::cat.Clip_Select(3, 0, 4, 5.0f);                       //Row, start col, frames, fps (Cat)
    
    // TODO Anim initialize
    //---------------------------------------

    camera.Magnitude() = 20.0f;

    camera.Position().x = base_player.Position().x;
    camera.Position().y = base_player.Position().y;
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    // Enemy
    enemy_cat.Init(base_player.Position());


	// Obstacles
    for (Obstacle& obstacle : obstacles) {
        obstacle.position.x = randFloat(0.0f, AEGfxGetWinMaxX() * 2.0f);
        obstacle.position.y = randFloat(AEGfxGetWinMinY() + 50.0f, AEGfxGetWinMaxY() - 50.0f);
        f32 random_size = randFloat(25.0f, 65.0f);
        obstacle.half_size.x = random_size;
        obstacle.half_size.y = random_size;
        obstacle.velocity.x = randFloat(-80.0f, 80.0f);
        obstacle.velocity.y = randFloat(-80.0f, 80.0f);
    }

    // Portal
    gMazePortal.half_size = { 30.0f, 60.0f };
    SpawnPortalAhead();
}

void Playing_Update() {
	// Check if window is closed
    if (0 == AESysDoesWindowExist()) {
        next = GAME_STATE_QUIT;
        return;
	}

    if (AEInputCheckTriggered(AEVK_P)) { isPaused = !isPaused; } // toggle pause

    if (isPaused) {
        // Destructive confirmation for quitting the game
        if (quitting_flag == TRUE) {
            // Click Y to quit, N to cancel
            if (AEInputCheckTriggered(AEVK_Y)) {
                // Quitting the game
                next = GAME_STATE_MENU;
            }
            else if (AEInputCheckTriggered(AEVK_N)) {
                quitting_flag = FALSE;
            }
        }
        // ==================================================


        if (AEInputCheckTriggered(AEVK_R)) {
            // return back to the game
            quitting_flag = FALSE;
            isPaused = false;
        }
        else if (AEInputCheckTriggered(AEVK_1))
        {
            next = GAME_STATE_RESTART; isPaused = false;
        }
        else if (AEInputCheckTriggered(AEVK_2))
        {
            //next = GAME_STATE_MENU;
            quitting_flag = TRUE;
        }
        /*else if (AEInputCheckTriggered(AEVK_3))
        {
            next = GAME_STATE_QUIT;
            quitting_flag = TRUE;
        }*/
        else {
            //next = GAME_STATE_RESTART;
        }
        return;
    }

    f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
    bool isFlying = (AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_SPACE) 
        || AEInputCheckCurr(AEVK_UP) ) && pFuel->HasFuel();

    if (pFuel) pFuel->Update(delta_time, isFlying);

    // Cat Update — after player movement
    enemy_cat.Update(delta_time, base_player.Position());

    if (enemy_cat.CheckPlayerCollision(base_player.Position(), base_player.Half_Size())
        && damage_timer <= 0.0f)
    {
        base_player.Health() -= base_player.Health(); // instant kill like Wall
        damage_timer = k_damage_cooldown;
        camera.Set_Shaking();
        graphics::particleInit(base_player.Position().x,
            base_player.Position().y, 50);
    }



    //Animation______________________________
    //ANIMATION::background.Anim_Update(delta_time);
    ANIMATION::asteroid.Anim_Update(delta_time);
    ANIMATION::player.Anim_Update(delta_time);
	ANIMATION::cat.Anim_Update(delta_time);
    // TODO Anim Update
    //---------------------------------------


    stage_timer += delta_time;
    if (damage_timer > 0.0f) damage_timer -= delta_time;

    // Obstacle Spawn after certain amt of time has passed
    if (obstacle_last_spawn_time > k_obstacle_spawn_cooldown) {
        obstacle_last_spawn_time = 0;
        // Create obj here as more and more but want to limit it too
        if (obstacles.size() < k_obstacle_count) {
            obstacles.push_back(Obstacle());
        }
    }
    else {
        obstacle_last_spawn_time += delta_time;
    }
    // ========================================================================

    if (pFuel) {
        base_player.Movement(delta_time, isFlying);
    }

	camera.Follow(base_player.Position());
    camera.Update();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    //Portal Respawn Time
    if (gPortalWaitingToRespawn)
    {
        gPortalRespawnTimer -= delta_time;

        if (gPortalRespawnTimer <= 0.0f)
        {
            if (gPortalRespawnTimer <= 0.0f)
            {
                SpawnPortalAhead();
            }
        }
    }


    f32 camX = camera.Position().x;
    // change this to based on player world pos
	f32 offscreen_limit_left{ AEGfxGetWinMinX() - 100.0f };
    for (Obstacle& obstacle : obstacles) {
        obstacle.position.x += obstacle.velocity.x * delta_time;
        obstacle.position.y += obstacle.velocity.y * delta_time;

        if (obstacle.position.y > AEGfxGetWinMaxY() - 20.0f) obstacle.velocity.y *= -1;
        if (obstacle.position.y < AEGfxGetWinMinY() + 20.0f) obstacle.velocity.y *= -1;

        if (obstacle.position.x < offscreen_limit_left) {
            obstacle.Reset();
        }
    }

    if (gMazePortal.active)
    {
        if (gMazePortal.pos.x < camera.Position().x - 1000.0f)
        {
            if (gMazePortal.pos.x < camera.Position().x - 1000.0f)
            {
                gMazePortal.active = false;
                gPortalWaitingToRespawn = true;
                gPortalRespawnTimer = 1.5f;
            }
        }

        if (checkOverlap(&(base_player.Position()), &(base_player.Half_Size()), &gMazePortal.pos, &gMazePortal.half_size))
        {
            SavePlayingStateForMaze();
            gMazeFromPlaying = true;
            gResumeFromMaze = true;
            gMazeCompleted = false;
            gMazeSuccess = false;

            gMazePortal.active = false;
            gPortalWaitingToRespawn = true;
            gPortalRespawnTimer = kPortalRespawnDelay;

            next = GAME_STATE_MAZE;
            return;
        }
    }

    if (pFuel) {
        // Timer-based fuel pickup spawn: every 10-20 seconds
        if (!g_fuel_pickup.active) {
            g_fuel_spawn_timer += delta_time;
            if (g_fuel_spawn_timer >= g_fuel_spawn_interval) {
                f32 spawnOffset = AEGfxGetWinMaxX() + 200.0f;
                if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
                    spawnOffset = -AEGfxGetWinMaxX() - 200.0f;
                }
                g_fuel_pickup.pos.x = camX + spawnOffset;
                g_fuel_pickup.pos.y = randFloat(AEGfxGetWinMinY() + 50.0f, AEGfxGetWinMaxY() - 50.0f);
                g_fuel_pickup.active = true;
                g_fuel_spawn_timer = 0.0f;
                g_fuel_spawn_interval = randFloat(10.0f, 20.0f);
            }
        }

        if (g_fuel_pickup.active) {
            AEVec2 p_half = { g_fuel_pickup.size / 2.0f, g_fuel_pickup.size / 2.0f };
            if (checkOverlap(&base_player.Position(), &base_player.Half_Size(), &g_fuel_pickup.pos, &p_half)) {
                pFuel->RestoreFuel(pFuel->GetMaxFuel() * Upgrades_GetFuelRestorePercentage());
                g_fuel_pickup.active = false;
                g_fuel_spawn_timer = 0.0f;
                g_fuel_spawn_interval = randFloat(10.0f, 20.0f);
            }
            if (std::abs(g_fuel_pickup.pos.x - camX) > AEGfxGetWinMaxX() + 500.0f) {
                g_fuel_pickup.active = false;
            }
        }
    }

	// check for obstacle-obstacle collisions and reset if they overlap to prevent unfair scenarios for the player
    for (int i = 0; i < obstacles.size(); i++) {
        for (int j = i + 1; j < obstacles.size(); j++) {
            if (checkOverlap(&obstacles[i].position, &obstacles[i].half_size,
                &obstacles[j].position, &obstacles[j].half_size)) {

				// if one is an asteroid then we want to reset both to prevent unfair scenarios for the player
                if (obstacles[i].type == Asteroid || obstacles[j].type == Asteroid) {
                    graphics::particleInit(obstacles[i].Position().x, obstacles[i].Position().y, 50);
                    obstacles[i].Reset();

                    graphics::particleInit(obstacles[j].Position().x, obstacles[j].Position().y, 50);
                    obstacles[j].Reset();
                }
                else {
					// if both are spikes/walls then we just reset without any particles since no need for visual
					obstacles[i].Reset();
					obstacles[j].Reset();
				}
            }
        }
    }
    

	// Check for collisions of player with obstacles, if collide then take damage and set damage timer
    bool took_damage = false;
    for (Obstacle const& obstacle : obstacles) {
        if (checkOverlap(&(base_player.Position()), &(base_player.Half_Size()), &obstacle.position, &obstacle.half_size)) {
            if (damage_timer <= 0.0f) {
                ratsqueakAudio.Play();

                if (obstacle.type == Wall) {// wall kill instantly
					base_player.Health() -= base_player.Health();
                }
                else {base_player.Health() -= 1;}
                
                damage_timer = k_damage_cooldown;
                took_damage = true;
            }
        }


    }

    if (took_damage) {
        camera.Set_Shaking();
        Credits_OnDamage();
        graphics::particleInit(base_player.Position().x, base_player.Position().y, 25);

        // IF take damage then we want to destroy all obstacles 
        for (Obstacle obstacle : obstacles) {
            graphics::particleInit(obstacle.Position().x, obstacle.Position().y, 50);
        }
        obstacles.clear();
    }

    bool game_active = (base_player.Health() > 0) && (stage_timer < k_stage_duration);
    Credits_Update(delta_time, game_active);

    if (base_player.Health() <= 0) next = GAME_STATE_GAME_OVER;

    //Changing game states
    // 
    // Current credits minus credits at the start of the round
    // outside of if condition to update during game
    credits_this_round = gTotalScore + (Credits_GetBalance() - Credits_GetInitBalance());
    if (current != next) {
        std::cout << "Finish Playing Credits " << credits_this_round << '\n';
        // Save current cheese score
        Credits_SaveFile("Assets/data/Cheese.txt");
    }
    else if (stage_timer >= k_stage_duration) next = GAME_STATE_VICTORY;
    else if (0 == AESysDoesWindowExist()) next = GAME_STATE_QUIT;

}

void Playing_Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

    f32 bg_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    f32 bg_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    f32 cam_x = camera.Position().x;
    f32 frame_width = 1.0f / 4.0f;


    f32 scroll_offset = fmodf(cam_x, bg_width);
    if (scroll_offset < 0.0f) scroll_offset += bg_width;


    f32 base_x = cam_x - scroll_offset;
    int base_frame = ((int)floorf(base_x / bg_width) % 4 + 4) % 4;

    // anchor tiles relative to camera with smooth offset
    for (int i = -1; i <= 2; ++i) {
        f32 tile_x = base_x + i * bg_width;
        int frame = (base_frame + i + 4) % 4;  
        f32 uv_x = frame * frame_width;

        AEGfxTextureSet(ASSETS::backgroundAssets, uv_x, 0.0f);
        drawQuad(unit_square, tile_x, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 1.f);
    }
    // Draw Enemy Cat
    ANIMATION::cat.Anim_Draw(ASSETS::playerAssets);   //Draws CAT
    enemy_cat.Draw(unit_square);

    if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) {

        //Animation______________________________
        ANIMATION::player.Anim_Draw(ASSETS::playerAssets);     //Draws PLAYER
        //_______________________________________

        drawQuad(base_player.Mesh(), base_player.Position().x, base_player.Position().y, base_player.Half_Size().x * 2.0f, base_player.Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
    }

	

    f32 obj_rotation{};
    for (Obstacle const& obstacle : obstacles) {
        switch (obstacle.type) {
            case Asteroid:
                //Animation______________________________
                ANIMATION::asteroid.Anim_Draw(ASSETS::backgroundAssets);   //Draws ASTEROID
                //---------------------------------------
                obj_rotation = 0.0f;
                break;
        
            case Spike:
                //Animation______________________________
                ANIMATION::spike.Anim_Draw(ASSETS::backgroundAssets);   //Draws SPIKE
                //---------------------------------------
                obj_rotation = obstacle.rotation;
                break;
            case Wall:
                //Animation______________________________
                ANIMATION::wall.Anim_Draw(ASSETS::backgroundAssets);   //Draws Wall
                //---------------------------------------
                obj_rotation = obstacle.rotation;
				break;
        }

        drawQuad(unit_square, obstacle.position.x, obstacle.position.y, obstacle.half_size.x * 2.0f, obstacle.half_size.y * 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, obj_rotation);

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

    if (gMazePortal.active)
    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxTextureSet(NULL, 0.0f, 0.0f);
        drawQuad(unit_square,
            gMazePortal.pos.x,
            gMazePortal.pos.y,
            gMazePortal.half_size.x * 2.0f,
            gMazePortal.half_size.y * 2.0f,
            0.4f, 0.0f, 1.0f, 1.0f);
    }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    drawHealthBar(unit_square, base_player, getMaxHealthFromUpgrades());

    char cheese_text[64];
    sprintf_s(cheese_text, "Cheese: %d", Credits_GetBalance());
    AEGfxPrint(ASSETS::Font(), cheese_text, -0.95f, 0.75f, 1.0f, 0.9f, 0.9f, 0.2f, 1.0f);

    char score_text[64];
    sprintf_s(score_text, "Score: %d", credits_this_round);
    AEGfxPrint(ASSETS::Font(), score_text, -0.95f, 0.85f, 1.0f, 0.9f, 0.9f, 0.2f, 1.0f);

    char fps_text[64];
    sprintf_s(fps_text, "%.1f FPS", 1 / (f32)AEFrameRateControllerGetFrameTime());
    AEGfxPrint(ASSETS::Font(), fps_text, -0.95f, 0.65f, 1.0f, .0f, .0f, .0f, 1.0f);

    /*AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    char timer_text[64];
    f32 time_left = k_stage_duration - stage_timer;
    if (time_left < 0.0f) time_left = 0.0f;
    sprintf_s(timer_text, "TIME LEFT: %.1f", time_left);
    AEGfxPrint(ASSETS::Font(), timer_text, -0.95f, 0.85f, 0.45f, 0.9f, 0.9f, 0.9f, 1.0f);*/

    graphics::particleDraw(unit_circle);

    if (!isPaused) return;

    float camX = camera.Position().x;
    float camY = camera.Position().y;

    // draw overlay
    drawQuad(bgMesh, camX, camY, 320.f, 220.f, 0.06f, 0.07f, 0.09f, 1.f);
    // draw text
    /*drawCenteredText(font_id, "PAUSED", 0.25f, 1.0f);
    drawCenteredText(font_id, "RETURN BACK TO GAME (R)", 0.05f, 0.7f);
    drawCenteredText(font_id, "RESTART (1)", -0.1f, 0.7f);*/
    //drawCenteredText(font_id, "RETURN TO MAIN MENU (2)", -0.2f, 0.7f);
    //drawCenteredText(font_id, "EXIT (3)", -0.3f, 0.7f);
    /*if (quitting_flag == TRUE) {
        drawCenteredText(font_id, "ARE YOU SURE YOU WANT TO GO BACK? (Y/N)", -0.4f, 0.7f);
    }*/

    if (quitting_flag == TRUE) {
        drawCenteredText(font_id, "ARE YOU SURE YOU WANT TO GO BACK? (Y/N)", 0.0f, 0.7f);
    }
    else {
        drawCenteredText(font_id, "PAUSED", 0.25f, 1.0f);
        drawCenteredText(font_id, "RETURN BACK TO GAME (R)", 0.05f, 0.7f);
        drawCenteredText(font_id, "RESTART (1)", -0.1f, 0.7f);
        drawCenteredText(font_id, "RETURN TO MAIN MENU (2)", -0.25f, 0.7f);
    }

    
}

void Playing_Free() {
    if (unit_square)  { AEGfxMeshFree(unit_square);  unit_square  = nullptr; }
    if (unit_circle)  { AEGfxMeshFree(unit_circle);  unit_circle  = nullptr; }
    if (bgMesh)       { AEGfxMeshFree(bgMesh);        bgMesh       = nullptr; }
    // Free the player mesh separately (base_player is static, its dtor runs at shutdown
    // which is too late; free here so restarts don't accumulate leaked meshes)
    if (base_player.Mesh()) { AEGfxMeshFree(base_player.Mesh()); base_player.Mesh() = nullptr; }
    if (pFuel) { delete pFuel; pFuel = nullptr; }


    obstacles.clear();
    obstacles.shrink_to_fit(); // actually releases the heap allocation
    gSavedPlayingState.obstacles.clear();
    gSavedPlayingState.obstacles.shrink_to_fit();
}

void Playing_Unload() {
    ASSETS::Unload_Images();
    ASSETS::Unload_Font();
    isPaused = false;
}

int getMaxHealthFromUpgrades() {
    f32 increase = Upgrades_GetHealthIncrease();
    f32 multiplier = 1.0f + increase;
    int max_health = (int)floorf(base_player.Config().MaxHealth() * multiplier);
    if (max_health < 1) max_health = 1;
    return max_health;
}