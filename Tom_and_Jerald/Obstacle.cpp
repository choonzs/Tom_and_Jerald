#include "pch.h"
#include "playing.h"
#include "utils.h"
#include "Obstacle.h"

void resetObstacle(Obstacle* obstacle) {
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();
	f32 size_value = randomRange(25.0f, 70.0f);
	AEVec2Set(&obstacle->half_size, size_value, size_value);
	AEVec2Set(
		&obstacle->position,
		//move it a little offscreen
		max_x + obstacle->half_size.x,
		randomRange(min_y + size_value, max_y - size_value));
	AEVec2Set(
		&obstacle->velocity,
		randomRange(-220.0f, 220.0f),
		randomRange(-220.0f, 220.0f));
}

void updateObstacles(Obstacle* Obstacles, f32 delta_time)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		Obstacle* obstacle = &Obstacles[i];
		obstacle->position.x += obstacle->velocity.x * delta_time;
		obstacle->position.y += obstacle->velocity.y * delta_time;

		// Once it reach off screen on the left, reset the obstacle
		if (obstacle->position.x - obstacle->half_size.x < min_x - (3 * obstacle->half_size.x))
		{
			// Reset obj positon
			resetObstacle(obstacle);
		}

		/*if (obstacle->position.x - obstacle->half_size.x < min_x)
		{
			obstacle->position.x = min_x + obstacle->half_size.x;
			obstacle->velocity.x *= -1.0f;
		}*/

		// Bounce off walls right boundary
		if (obstacle->position.x + obstacle->half_size.x > max_x + (3 * obstacle->half_size.x))
		{
			obstacle->velocity.x *= -1.0f;
		}

		// Bounce off walls bottom boundary
		if (obstacle->position.y - obstacle->half_size.y < min_y)
		{
			obstacle->position.y = min_y + obstacle->half_size.y;
			obstacle->velocity.y *= -1.0f;
		}

		// Bounce off walls top boundary
		if (obstacle->position.y + obstacle->half_size.y > max_y)
		{
			obstacle->position.y = max_y - obstacle->half_size.y;
			obstacle->velocity.y *= -1.0f;
		}
	}
}