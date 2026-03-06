#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP
#include "pch.hpp"
enum ObstacleType : int {Non_Obstacle = 0,Asteroid = 1, Spike = 2};

class Obstacle
{
public:
    ObstacleType type{};
    AEVec2 position{};
    AEVec2 velocity{};
    AEVec2 half_size{};
	Obstacle() = default;
	Obstacle(ObstacleType type, AEVec2 pos, AEVec2 velocity, AEVec2 half_size) 
		: type{ type }, position{ pos },
		velocity{ velocity }, half_size{ half_size } {}
	
    void Reset();
    void Update(f32 delta_time, f32 cam_x, f32 offscreen_limit, bool endless);
	
	// Accessor & Mutator
	ObstacleType& Type() { return type; }
	ObstacleType const& Type() const { return type; }

	AEVec2& Position() { return position; }
	AEVec2 const& Position() const { return position; }
	AEVec2 const* PositionPtr() const { return &position; }

	AEVec2& Velocity() { return velocity; }
	AEVec2 const& Velocity() const { return velocity; }
	AEVec2 const* VelocityPtr() const { return &velocity; }

	AEVec2& HalfSize() { return half_size; }
	AEVec2 const& HalfSize() const { return half_size; }
	AEVec2 const* HalfSizePtr() const { return &half_size; }

};

class ObstacleSystem
{
public:
	ObstacleSystem() : obstacles(0) {};

    static void ResetObstacle(Obstacle* obstacle);
    //static void UpdateObstacles(Obstacle* obstacles, f32 delta_time);

	// Add to vector of obstacles
	void AddObstacle(Obstacle& obstacle) { obstacles.push_back(obstacle); }

    //Accessor & Mutator
	std::vector<Obstacle>& Obstacles() { return obstacles; }
	const std::vector<Obstacle>& Obstacles() const { return obstacles; }

private:
	std::vector<Obstacle> obstacles;
};

void resetObstacle(Obstacle* obstacle);
//void updateObstacles(Obstacle* obstacles, f32 delta_time);

#endif // !OBSTACLE_HPP
