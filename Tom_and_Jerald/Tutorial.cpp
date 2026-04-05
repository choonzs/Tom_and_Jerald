/*************************************************************************
@file    Tutorial.cpp
@Author  Peng Rong Jun Jerald p.rongjunjerald@digipen.edu
@Co-authors  Loh Kai Xin kaixin.l@digipen.edu
@brief
     Implements the Tutorial state: step-by-step introduction to
     jetpack movement, fuel management, and obstacle avoidance.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#include "pch.hpp"
#include "Tutorial.hpp"

#include "Utils.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Player.hpp"
#include "JetpackFuel.hpp"
#include "Obstacle.hpp"
#include "Animation.hpp"
#include "ImgFontInit.hpp"
#include "Audio.hpp"
#include "Camera.hpp"

namespace {
    enum TutorialStep
    {
        TUT_MOVE = 0,
        TUT_FLY,
        TUT_OUT_OF_FUEL,
        TUT_SPAWN_ASTEROID,
        TUT_AVOID_ASTEROID,
        TUT_SUCCESS,
        TUT_RETRY
    };

    AEGfxVertexList* unit_square = nullptr;
    AEGfxVertexList* unit_circle = nullptr;
    AEGfxVertexList* bgMesh = nullptr;

    Player tutorial_player;
    Camera camera;
    JetpackFuel* pFuel = nullptr;
    Obstacle tutorial_asteroid{};

    TutorialStep step = TUT_MOVE;

    bool moved_with_wasd = false;
    bool fuel_drained = false;
    bool asteroid_spawned = false;
    bool player_was_hit = false;

    f32 message_timer = 0.0f;
    f32 damage_timer = 0.0f;
    f32 window_width = 0.0f;
    f32 window_height = 0.0f;

    s8 font_id = -1;
}

static void ResetAsteroid()
{
    tutorial_asteroid.Reset();
    tutorial_asteroid.type = Asteroid;
    tutorial_asteroid.half_size.x = 35.0f;
    tutorial_asteroid.half_size.y = 35.0f;
    tutorial_asteroid.position.x = tutorial_player.Position().x + 900.0f;
    tutorial_asteroid.position.y = tutorial_player.Position().y;
    tutorial_asteroid.velocity.x = -350.0f;
    tutorial_asteroid.velocity.y = 0.0f;
    asteroid_spawned = true;
    player_was_hit = false;
}

void Tutorial_Load()
{
    ASSETS::Init_Images();
    ASSETS::Init_Font();
    font_id = ASSETS::Font();

    ANIMATION::background.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::background.Clip_Select(0, 0, 4, 1.0f);

    ANIMATION::player.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::player.Clip_Select(0, 0, 2, 10.0f);

    ANIMATION::asteroid.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::asteroid.Clip_Select(2, 0, 2, 10.0f);
}

void Tutorial_Initialize()
{
    createUnitSquare(&unit_square, 0.25f, 0.25f);
    createUnitCircles(&unit_circle);
    createUnitSquare(&(tutorial_player.Mesh()), 0.25f, 0.25f);
    createUnitSquare(&bgMesh, 0.25f, 0.25f, 1.0f);

    window_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    window_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

    tutorial_player.Position().x = 0.0f;
    tutorial_player.Position().y = 0.0f;
    tutorial_player.Velocity().x = 0.0f;
    tutorial_player.Velocity().y = 0.0f;
    tutorial_player.Half_Size().x = 40.0f;
    tutorial_player.Half_Size().y = 40.0f;
    tutorial_player.Health() = 3;

    if (pFuel) delete pFuel;
    pFuel = new JetpackFuel(100.0f, 15.0f, 1.0f); //Drains faster for tutorial ONLY

    step = TUT_MOVE;
    moved_with_wasd = false;
    fuel_drained = false;
    asteroid_spawned = false;
    player_was_hit = false;
    message_timer = 0.0f;
    damage_timer = 0.0f;

    camera.Magnitude() = 20.0f;
    camera.Position().x = tutorial_player.Position().x;
    camera.Position().y = tutorial_player.Position().y;
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
    AEInputShowCursor(false);
}

void Tutorial_Update()
{
    const f32 dt = (f32)AEFrameRateControllerGetFrameTime();

    if (damage_timer > 0.0f)
        damage_timer -= dt;

    bool isFlying =
        (AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_SPACE) || AEInputCheckCurr(AEVK_UP)) &&
        pFuel->HasFuel();

    if (pFuel)
        pFuel->Update(dt, isFlying);

    tutorial_player.Movement(dt, isFlying);

    camera.Follow(tutorial_player.Position());
    camera.Update();
    AEGfxSetCamPosition(camera.Position().x, camera.Position().y);

    ANIMATION::background.Anim_Update(dt);
    ANIMATION::player.Anim_Update(dt);
    ANIMATION::asteroid.Anim_Update(dt);

    switch (step)
    {
    case TUT_MOVE:
        if (AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_A) ||
            AEInputCheckCurr(AEVK_S) || AEInputCheckCurr(AEVK_D))
        {
            moved_with_wasd = true;
            step = TUT_FLY;
        }
        break;

    case TUT_FLY:
        if (pFuel->GetCurrentFuel() < pFuel->GetMaxFuel())
        {
            fuel_drained = true;
            step = TUT_OUT_OF_FUEL;
        }
        break;

    case TUT_OUT_OF_FUEL:
        if (pFuel->GetCurrentFuel() <= 0.0f)
        {
            step = TUT_SPAWN_ASTEROID;
        }
        break;

    case TUT_SPAWN_ASTEROID:
        pFuel->RestoreFuel(pFuel->GetMaxFuel());
        ResetAsteroid();
        step = TUT_AVOID_ASTEROID;
        break;

    case TUT_AVOID_ASTEROID:
        tutorial_asteroid.position.x += tutorial_asteroid.velocity.x * dt;

        if (checkOverlap(&tutorial_player.Position(), &tutorial_player.Half_Size(),
            &tutorial_asteroid.position, &tutorial_asteroid.half_size))
        {
            if (damage_timer <= 0.0f)
            {
                tutorial_player.Health() -= 1;
                player_was_hit = true;
                damage_timer = 1.0f;
                step = TUT_RETRY;
                message_timer = 1.0f;
            }
        }

        if (tutorial_asteroid.position.x < tutorial_player.Position().x - window_width * 0.75f &&
            !player_was_hit)
        {
            step = TUT_SUCCESS;
            message_timer = 1.0f;
        }
        break;

    case TUT_RETRY:
        message_timer -= dt;
        if (message_timer <= 0.0f)
        {
            tutorial_player.Health() = 3;
            tutorial_player.Position().x = camera.Position().x;
            tutorial_player.Position().y = 0.0f;
            tutorial_player.Velocity().x = 0.0f;
            tutorial_player.Velocity().y = 0.0f;

            if (pFuel) delete pFuel;
            pFuel = new JetpackFuel(100.0f, 15.0f, 1.0f);

            fuel_drained = false;
            asteroid_spawned = false;
            player_was_hit = false;
            step = TUT_SPAWN_ASTEROID;
        }
        break;

    case TUT_SUCCESS:
        message_timer -= dt;
        if (message_timer <= 0.0f)
            next = GAME_STATE_PLAYING;
        break;
    }

    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        gSkipSplash = true;
        next = GAME_STATE_MENU;
    }
}

void Tutorial_Draw()
{
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

    f32 bg_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    f32 bg_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    f32 cam_x = camera.Position().x;
    f32 frame_width = 1.0f / 4.0f;

    f32 scroll_offset = fmodf(cam_x, bg_width);
    if (scroll_offset < 0.0f) scroll_offset += bg_width;

    f32 base_x = cam_x - scroll_offset;
    int base_frame = ((int)floorf(base_x / bg_width) % 4 + 4) % 4;

    for (int i = -1; i <= 2; ++i) {
        f32 tile_x = base_x + i * bg_width;
        int frame = (base_frame + i + 4) % 4;
        f32 uv_x = frame * frame_width;

        AEGfxTextureSet(ASSETS::backgroundAssets, uv_x, 0.0f);
        drawQuad(unit_square, tile_x, camera.Position().y, bg_width, bg_height, 1.f, 1.f, 1.f, 0.7f);
    }

    ANIMATION::player.Anim_Draw(ASSETS::playerAssets);
    drawQuad(tutorial_player.Mesh(),
        tutorial_player.Position().x,
        tutorial_player.Position().y,
        tutorial_player.Half_Size().x * 2.0f,
        tutorial_player.Half_Size().y * 2.0f,
        1.0f, 1.0f, 1.0f, 1.0f);

    if (step == TUT_AVOID_ASTEROID || step == TUT_SUCCESS || step == TUT_RETRY)
    {
        ANIMATION::asteroid.Anim_Draw(ASSETS::backgroundAssets);
        drawQuad(unit_square,
            tutorial_asteroid.position.x,
            tutorial_asteroid.position.y,
            tutorial_asteroid.half_size.x * 2.0f,
            tutorial_asteroid.half_size.y * 2.0f,
            1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (pFuel)
    {
        pFuel->Draw(
            tutorial_player.Position().x,
            tutorial_player.Position().y + tutorial_player.Half_Size().y + 20.0f,
            60.0f, 10.0f
        );
    }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.0f, 0.0f);

    switch (step)
    {
    case TUT_MOVE:
        drawCenteredText(font_id, "PRESS WASD TO MOVE", 0.7f, 1.2f);
        break;
    case TUT_FLY:
        drawCenteredText(font_id, "PRESS W TO FLY", 0.7f, 1.2f);
        drawCenteredText(font_id, "FLYING DRAINS FUEL", 0.6f, 1.1f);
        break;
    case TUT_OUT_OF_FUEL:
        drawCenteredText(font_id, "KEEP FLYING UNTIL FUEL RUNS OUT", 0.7f, 1.0f);
        drawCenteredText(font_id, "WHEN FUEL IS 0, YOU CANNOT FLY", 0.6f, 1.0f);
        break;
    case TUT_SPAWN_ASTEROID:
    case TUT_AVOID_ASTEROID:
        drawCenteredText(font_id, "AN ASTEROID IS COMING!", 0.7f, 1.2f);
        drawCenteredText(font_id, "TRY TO AVOID IT", 0.6f, 1.2f);
        break;
    case TUT_SUCCESS:
        drawCenteredText(font_id, "GOOD JOB!", 0.7f, 1.4f);
        break;
    case TUT_RETRY:
        drawCenteredText(font_id, "OOPS... LET'S TRY THAT AGAIN", 0.7f, 1.1f);
        break;
    }
}

void Tutorial_Free()
{
    if (unit_square) { AEGfxMeshFree(unit_square); unit_square = nullptr; }
    if (unit_circle) { AEGfxMeshFree(unit_circle); unit_circle = nullptr; }
    if (bgMesh) { AEGfxMeshFree(bgMesh); bgMesh = nullptr; }
    if (tutorial_player.Mesh()) { AEGfxMeshFree(tutorial_player.Mesh()); tutorial_player.Mesh() = nullptr; }
    if (pFuel) { delete pFuel; pFuel = nullptr; }
}

void Tutorial_Unload()
{
    ASSETS::Unload_Images();
    ASSETS::Unload_Font();
}