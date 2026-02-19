#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "pch.hpp"

class Obstacle
{
public:
    AEVec2 position{};
    AEVec2 velocity{};
    AEVec2 half_size{};

    void Reset();
    void Update(f32 delta_time, f32 min_x, f32 max_x, f32 min_y, f32 max_y);
};

class ObstacleSystem
{
public:
    static void ResetObstacle(Obstacle* obstacle);
    static void UpdateObstacles(Obstacle* obstacles, f32 delta_time);
};

void resetObstacle(Obstacle* obstacle);
void updateObstacles(Obstacle* obstacles, f32 delta_time);

#endif // !OBSTACLE_HPP
