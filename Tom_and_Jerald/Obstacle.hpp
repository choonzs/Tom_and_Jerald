#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP
struct Obstacle
{
	AEVec2 position;
	AEVec2 velocity;
	AEVec2 half_size;
};

void resetObstacle(Obstacle* obstacle);
void updateObstacles(Obstacle* Obstacles, f32 delta_time);
#endif // !OBSTACLE_HPP

