/*************************************************************************
@file Obstacle.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu
@brief
    Define the Obstacle struct, ObstacleSystem manager, and supporting
    enums for obstacle type, scale, and speed.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

// Obstacle.cpp
// ---------------------------------------------------------------------------
// Implements obstacle behavior: spawning, movement, bouncing, and
// offscreen recycling. Obstacles are the core hazards in the game.
// ---------------------------------------------------------------------------

#include "pch.hpp"
#include "playing.hpp"
#include "utils.hpp"
#include "Obstacle.hpp"

// ---------------------------------------------------------------------------
// Obstacle::Reset
// ---------------------------------------------------------------------------
// Randomizes this obstacle for reuse in endless mode. Called when an
// obstacle is constructed or scrolls offscreen.
//
// Asteroid spawn rules:
//   - Position X: just offscreen to the right (max_x + half_size.x).
//   - Position Y: random within screen bounds, inset by size to prevent
//     clipping through edges.
//   - Size: random square between 25–70 pixels.
//   - Velocity X: random between -220 and 0 (always moves left).
//   - Velocity Y: random between -220 and +220 (creates diagonal movement).
//
// Spike / Wall spawn rules:
//   - Same random size as Asteroid.
//   - Spawns flush against either the ceiling or floor (random).
//   - Ceiling obstacles are rotated PI to point downward; floor ones are 0.
//   - Velocity is always zero (static hazards).
// ---------------------------------------------------------------------------
void Obstacle::Reset()
{
    const f32 max_x = AEGfxGetWinMaxX() + 200.0f;
    const f32 min_y = AEGfxGetWinMinY();
    const f32 max_y = AEGfxGetWinMaxY();
    const f32 size_value = randomRange(25.0f, 70.0f);

    type = randomRange(ObstacleType::Asteroid, ObstacleType::Wall);

    switch (type)
    {
    case Asteroid:
        AEVec2Set(&half_size, size_value, size_value);
        AEVec2Set(
            &position,
            max_x + half_size.x,
            randomRange(min_y + size_value + 50.0f, max_y - size_value));
        AEVec2Set(
            &velocity,
            randomRange(-220.0f, 0.0f),
            randomRange(-220.0f, 220.0f));
        rotation = 0.0f;
        break;

    case Wall:  // Falls through: Wall and Spike share identical spawn logic.
    case Spike:
        AEVec2Set(&half_size, size_value, size_value);
        AEVec2Set(&velocity, 0.0f, 0.0f);

        if (rand() % 2)
        {
            // Ceiling: position flush to top edge, rotated to point down.
            AEVec2Set(&position, max_x + half_size.x, max_y - half_size.y);
            rotation = PI;
        }
        else
        {
            // Floor: position flush to bottom edge, no rotation.
            AEVec2Set(&position, max_x + half_size.x, min_y + half_size.y + 50.0f);
            rotation = 0.0f;
        }
        break;
    }
}

// ---------------------------------------------------------------------------
// Obstacle::Update
// ---------------------------------------------------------------------------
// Per-frame update for a single obstacle. Handles:
//   1. Position integration: position += velocity * dt.
//   2. Vertical bounce off screen top/bottom (20 px inset buffer).
//   3. Offscreen detection:
//      - Endless mode:      Reset() so the obstacle re-enters from the right.
//      - Custom level mode: Mark as Non_Obstacle so the caller can erase it.
//
// Bounce direction:
//   When the obstacle crosses the top or bottom threshold, velocity.y is
//   forced to -abs(velocity.y), which always pushes it back toward center.
//   This avoids sign errors that can trap an obstacle against an edge.
// ---------------------------------------------------------------------------
void Obstacle::Update(f32 dt, f32 camX, f32 offscreen_limit, bool endless)
{
    // Integrate position.
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    // Bounce off top and bottom screen edges (with a 20 px inset buffer).
    const bool past_top = position.y > AEGfxGetWinMaxY() - 20.0f;
    const bool past_bottom = position.y < AEGfxGetWinMinY() + 20.0f + 50.0f;
    if (past_top || past_bottom)
        velocity.y = -std::abs(velocity.y);

    // Check whether the obstacle has scrolled off either side of the camera.
    const bool offscreen = (position.x < camX - offscreen_limit)
        || (position.x > camX + offscreen_limit);
    if (offscreen)
    {
        if (endless)
            Reset();           // Endless mode: recycle and respawn.
        else
            type = Non_Obstacle; // Custom level: mark for deletion.
    }
}

// ---------------------------------------------------------------------------
// ObstacleSystem::ResetObstacle
// ---------------------------------------------------------------------------
// Null-safe static helper that calls Reset() on the given obstacle pointer.
// ---------------------------------------------------------------------------
void ObstacleSystem::ResetObstacle(Obstacle* obstacle)
{
    if (obstacle)
        obstacle->Reset();
}

// ---------------------------------------------------------------------------
// resetObstacle (free function)
// ---------------------------------------------------------------------------
// Legacy wrapper around ObstacleSystem::ResetObstacle, kept for backward
// compatibility with older call sites.
// ---------------------------------------------------------------------------
void resetObstacle(Obstacle* obstacle)
{
    ObstacleSystem::ResetObstacle(obstacle);
}

// ===========================================================================
// ObstacleScale operators and helpers
// ===========================================================================

// Prefix -- : steps scale down, wrapping Giant back to Tiny.
ObstacleScale& operator--(ObstacleScale& scale)
{
    scale = (scale == ObstacleScale::Tiny)
        ? ObstacleScale::Giant
        : ObstacleScale(static_cast<int>(scale) - 1);
    return scale;
}

// Prefix ++ : steps scale up, wrapping Tiny forward to Giant.
ObstacleScale& operator++(ObstacleScale& scale)
{
    scale = (scale == ObstacleScale::Giant)
        ? ObstacleScale::Tiny
        : ObstacleScale(static_cast<int>(scale) + 1);
    return scale;
}

// Returns the pixel size multiplier for the given scale tier.
f32 GetObstacleSize(ObstacleScale const& scale)
{
    switch (scale)
    {
    case ObstacleScale::Tiny:   return 2.0f;
    case ObstacleScale::Small:  return 3.0f;
    case ObstacleScale::Normal: return 4.0f;
    case ObstacleScale::Large:  return 5.0f;
    case ObstacleScale::Giant:  return 6.0f;
    }
    return 4.0f; // Unreachable; satisfies compiler.
}

// ===========================================================================
// ObstacleSpeed operators and helpers
// ===========================================================================

// Prefix -- : steps speed down, wrapping Slow back to Fast.
ObstacleSpeed& operator--(ObstacleSpeed& speed)
{
    speed = (speed == ObstacleSpeed::Slow)
        ? ObstacleSpeed::Fast
        : ObstacleSpeed(static_cast<int>(speed) - 1);
    return speed;
}

// Prefix ++ : steps speed up, wrapping Fast forward to Slow.
ObstacleSpeed& operator++(ObstacleSpeed& speed)
{
    speed = (speed == ObstacleSpeed::Fast)
        ? ObstacleSpeed::Slow
        : ObstacleSpeed(static_cast<int>(speed) + 1);
    return speed;
}

// Returns the X velocity (always negative = leftward) for the given speed tier.
f32 GetObstacleSpeed(ObstacleSpeed const& speed)
{
    switch (speed)
    {
    case ObstacleSpeed::Slow:   return  -20.0f;
    case ObstacleSpeed::Normal: return  -50.0f;
    case ObstacleSpeed::Fast:   return -100.0f;
    }
    return -50.0f; // Unreachable; satisfies compiler.
}