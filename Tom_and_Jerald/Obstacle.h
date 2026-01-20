#ifndef OBSTACLE_H
#define OBSTACLE_H
struct Obstacle
{
	AEVec2 position;
	AEVec2 velocity;
	AEVec2 half_size;
};

void resetObstacle(Obstacle* obstacle);
void updateObstacles(Obstacle* Obstacles, f32 delta_time);
#endif // !OBSTACLE_H

