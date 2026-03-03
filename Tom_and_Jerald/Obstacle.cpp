#include "pch.hpp"
#include "playing.hpp"
#include "utils.hpp"
#include "Obstacle.hpp"

const int k_obstacle_count = 100;

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

void resetObstacle(Obstacle* obstacle)
{
    ObstacleSystem::ResetObstacle(obstacle);
}

//Not needed for now
//void updateObstacles(Obstacle* obstacles, f32 delta_time)
//{
//    ObstacleSystem::UpdateObstacles(obstacles, delta_time);
//}
