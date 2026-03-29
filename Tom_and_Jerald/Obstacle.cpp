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

    type = randomRange(ObstacleType::Asteroid, ObstacleType::Wall);
    std::cout << "Obstacle Type: " << type << std::endl;
    
    switch (type) {
    case Asteroid:
		rotation = 0.0f; // No rotation for asteroid
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
        break;
    case Wall: // fall through, similar behaviour
    case Spike: 
        AEVec2Set(&half_size, size_value, size_value);

        // Randomly place on ceiling or floor
        int on_ceiling = rand() % 2;

        if (on_ceiling) {
            // Draw on ceiling
            AEVec2Set(
                &position,
                max_x + half_size.x,
                max_y - half_size.y);
            rotation = PI;
        }
        else {
            // Draw on floor
            AEVec2Set(
                &position,
                max_x + half_size.x,
                min_y + half_size.y); 
            rotation = 0.0f;
        }

        AEVec2Set(&velocity, 0.0f, 0.0f); // static obstacle 
    }

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
// ==================================================================================
// 
// Negation of Obstacle Scale
ObstacleScale& operator--(ObstacleScale& scale) {
    scale = (scale == ObstacleScale::Tiny) ? ObstacleScale::Giant : ObstacleScale(static_cast<int> (scale) - 1);
    return scale;
}
// Addition of Obstacle Scale
ObstacleScale& operator++(ObstacleScale& scale) {
    scale = (scale == ObstacleScale::Giant) ? ObstacleScale::Tiny : ObstacleScale(static_cast<int> (scale) + 1);
    return scale;
}
// Obstacle Size values
f32 GetObstacleSize(ObstacleScale const& scale) {
    f32 result{};
    switch (scale) {
    case ObstacleScale::Tiny:
        result = 2.0f;
        break;
    case ObstacleScale::Small:
        result = 3.0f;
        break;
    case ObstacleScale::Normal:
        result = 4.0f;
        break;
    case ObstacleScale::Large:
        result = 5.0f;
        break;
    case ObstacleScale::Giant:
        result = 6.0f;
        break;
    }
    return result;
}
// =====================================================
// Negation of Obstacle Scale
ObstacleSpeed& operator--(ObstacleSpeed& speed) {
    speed = (speed == ObstacleSpeed::Slow) ? ObstacleSpeed::Fast : ObstacleSpeed(static_cast<int> (speed) - 1);
    return speed;
}
// Addition of Obstacle Scale
ObstacleSpeed& operator++(ObstacleSpeed& speed) {
    speed = (speed == ObstacleSpeed::Fast) ? ObstacleSpeed::Slow : ObstacleSpeed(static_cast<int> (speed) + 1);
    return speed;
}
f32 GetObstacleSpeed(ObstacleSpeed const& speed) {
    f32 result{};
    switch (speed) {
    case ObstacleSpeed::Slow:
        result = -20.0f;
        break;
    case ObstacleSpeed::Normal:
        result = -50.0f;
        break;
    case ObstacleSpeed::Fast:
        result = -100.0f;
        break;
    }
        return result;
}
// ===================================================================