// Obstacle.cpp
// ---------------------------------------------------------------------------
// Implements obstacle behavior: spawning, movement, bouncing, and
// offscreen recycling. Obstacles are the core hazards in the game.
// ---------------------------------------------------------------------------

#include "pch.hpp"
#include "playing.hpp"
#include "utils.hpp"
#include "Obstacle.hpp"

const int k_obstacle_count = 100;

// Obstacle::Reset
// ---------------------------------------------------------------------------
// Randomizes this obstacle for reuse in endless mode. Called when an
// obstacle goes offscreen to recycle it back into play.
//
// Spawning logic:
//   - Position X: spawns just offscreen to the right (max_x + half_size.x)
//     so it scrolls naturally into view.
//   - Position Y: random within screen bounds, offset by size to prevent
//     clipping through top/bottom edges.
//   - Size: random between 25-70 pixels (both width and height are equal,
//     creating square obstacles).
//   - Velocity X: random between -220 and 0 (always moves left toward player).
//   - Velocity Y: random between -220 and +220 (creates diagonal movement).
// ---------------------------------------------------------------------------
void Obstacle::Reset()
{
    // temp
    f32 max_x = AEGfxGetWinMaxX() + 200.0f;
    f32 min_y = AEGfxGetWinMinY();
    f32 max_y = AEGfxGetWinMaxY();
    f32 size_value = randomRange(25.0f, 70.0f);

    AEVec2Set(&half_size, size_value, size_value);
    AEVec2Set(
        &position,
        // move it a little offscreen
        max_x + half_size.x,
        randomRange(min_y + size_value, max_y - size_value));
    AEVec2Set(
        &velocity,
        randomRange(-220.0f, 0.0f),
        randomRange(-220.0f, 220.0f));
}

// Obstacle::Update
// ---------------------------------------------------------------------------
// Per-frame update for a single obstacle. Handles:
//   1. Position integration (position += velocity * dt)
//   2. Vertical bouncing off screen top/bottom edges
//   3. Offscreen detection and recycling/destruction
//
// Parameters:
//   dt              - Delta time (seconds) for frame-independent movement.
//   camX            - Current camera X position. Used as reference point
//                     for calculating offscreen distance.
//   offscreen_limit - Distance from camera center beyond which the
//                     obstacle is considered offscreen.
//   endless         - Gameplay mode flag:
//                     true  = Endless mode: Reset() the obstacle to spawn
//                             a new one from the right (infinite gameplay).
//                     false = Custom level: mark as Non_Obstacle so the
//                             caller can erase it from the vector.
//
// Bouncing:
//   When the obstacle's Y position exceeds the screen top minus a 20px
//   buffer, or falls below the screen bottom plus a 20px buffer, the
//   Y velocity is negated (flipped) to create a bounce effect. Uses
//   -abs() to ensure correct direction after flip.
// ---------------------------------------------------------------------------
void Obstacle::Update(f32 dt, f32 camX, f32 offscreen_limit, bool endless = true)
{
    // checks if the obstacle exists
    if (!this) { return; }
    //Update Position based on velocity
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    // Bounce off top and bottom of the screen
    velocity.y = ((position.y > AEGfxGetWinMaxY() - 20.0f) or (position.y < AEGfxGetWinMinY() + 20.0f))
        ? -std::abs(velocity.y) : velocity.y;


    if (position.x < camX - offscreen_limit or position.x > camX + offscreen_limit) {
        // Gamemode is endless so reset the obstacle to come back from the right side of the screen
        if (endless) {
            Reset();
        }
        else {
            // Gamemode not endless so destroy the obstacle 
            type = Non_Obstacle;
        }
    }
}

// ObstacleSystem::ResetObstacle
// ---------------------------------------------------------------------------
// Static helper that resets a single obstacle via pointer.
// Null-checks before calling Reset() to prevent crashes.
// ---------------------------------------------------------------------------
void ObstacleSystem::ResetObstacle(Obstacle* obstacle)
{
    if (obstacle)
    {
        obstacle->Reset();
    }
}
// Not needed for now
//void ObstacleSystem::UpdateObstacles(Obstacle* obstacles, f32 delta_time)
//{
//    if (!obstacles)
//    {
//        return;
//    }
//
//    f32 min_x = AEGfxGetWinMinX();
//    f32 max_x = AEGfxGetWinMaxX();
//    f32 min_y = AEGfxGetWinMinY();
//    f32 max_y = AEGfxGetWinMaxY();
//
//    // TODO can be replaced
//    /*for (int i = 0; i < k_obstacle_count; ++i)
//    {
//        obstacles[i].Update(delta_time, min_x, max_x, min_y, max_y);
//    }*/
//}

// resetObstacle (free function)
// ---------------------------------------------------------------------------
// Legacy wrapper that delegates to ObstacleSystem::ResetObstacle.
// Kept for backward compatibility with older code that uses free functions.
// ---------------------------------------------------------------------------
void resetObstacle(Obstacle* obstacle)
{
    ObstacleSystem::ResetObstacle(obstacle);
}

//Not needed for now
//void updateObstacles(Obstacle* obstacles, f32 delta_time)
//{
//    ObstacleSystem::UpdateObstacles(obstacles, delta_time);
//}