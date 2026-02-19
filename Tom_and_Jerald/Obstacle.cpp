#include "pch.hpp"
#include "playing.hpp"
#include "utils.hpp"
#include "Obstacle.hpp"

void Obstacle::Reset()
{
    f32 max_x = AEGfxGetWinMaxX();
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
        randomRange(-220.0f, 220.0f),
        randomRange(-220.0f, 220.0f));
}

void Obstacle::Update(f32 delta_time, f32 min_x, f32 max_x, f32 min_y, f32 max_y)
{
    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;

    // Once it reach off screen on the left, reset the obstacle
    if (position.x - half_size.x < min_x - (3 * half_size.x))
    {
        Reset();
    }

    // Bounce off walls right boundary
    if (position.x + half_size.x > max_x + (3 * half_size.x))
    {
        velocity.x *= -1.0f;
    }

    // Bounce off walls bottom boundary
    if (position.y - half_size.y < min_y)
    {
        position.y = min_y + half_size.y;
        velocity.y *= -1.0f;
    }

    // Bounce off walls top boundary
    if (position.y + half_size.y > max_y)
    {
        position.y = max_y - half_size.y;
        velocity.y *= -1.0f;
    }
}

void ObstacleSystem::ResetObstacle(Obstacle* obstacle)
{
    if (obstacle)
    {
        obstacle->Reset();
    }
}

void ObstacleSystem::UpdateObstacles(Obstacle* obstacles, f32 delta_time)
{
    if (!obstacles)
    {
        return;
    }

    f32 min_x = AEGfxGetWinMinX();
    f32 max_x = AEGfxGetWinMaxX();
    f32 min_y = AEGfxGetWinMinY();
    f32 max_y = AEGfxGetWinMaxY();

    for (int i = 0; i < k_obstacle_count; ++i)
    {
        obstacles[i].Update(delta_time, min_x, max_x, min_y, max_y);
    }
}

void resetObstacle(Obstacle* obstacle)
{
    ObstacleSystem::ResetObstacle(obstacle);
}

void updateObstacles(Obstacle* obstacles, f32 delta_time)
{
    ObstacleSystem::UpdateObstacles(obstacles, delta_time);
}
