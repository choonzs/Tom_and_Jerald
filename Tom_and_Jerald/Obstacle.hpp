// Obstacle.hpp
// ---------------------------------------------------------------------------
// Declares the Obstacle class and ObstacleSystem manager.
// Obstacles are the hazards the player must avoid in the side-scroller.
// They have types (Asteroid, Spike), positions, velocities, and sizes.
//
// ObstacleType Enum:
//   Non_Obstacle (0) - Marked for deletion (used in custom levels when
//                      an obstacle goes offscreen in non-endless mode).
//   Asteroid     (1) - Moving obstacle that bounces off top/bottom edges.
//   Spike        (2) - Static obstacle (velocity set to zero in custom levels).
//
// ObstacleSystem:
//   Manages a dynamic vector of Obstacle objects. Used primarily in
//   custom levels where obstacles are loaded from file. The endless
//   mode in Playing.cpp uses a fixed-size array instead.
// ---------------------------------------------------------------------------
#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP
#include "pch.hpp"
// ---------------------------------------------------------------------------
// Represents Obstacle Type
// ---------------------------------------------------------------------------
enum ObstacleType : int { Non_Obstacle = 0, Asteroid = 1, Spike = 2 };
// ---------------------------------------------------------------------------
// Represents Obstacle Type
// ---------------------------------------------------------------------------
enum class ObstacleScale : int {Tiny = 0, Small, Normal, Large, Giant};
// Negation of Obstacle Scale
ObstacleScale& operator--(ObstacleScale& scale);
// Addition of Obstacle Scale
ObstacleScale& operator++(ObstacleScale& scale);
f32 GetObstacleSize(ObstacleScale const& scale);
// ---------------------------------------------------------------------------
// Represents Obstacle Speed
// ---------------------------------------------------------------------------
enum class ObstacleSpeed : int {Slow, Normal, Fast};
// Negation of Obstacle Scale
ObstacleSpeed& operator--(ObstacleSpeed& speed);
// Addition of Obstacle Scale
ObstacleSpeed& operator++(ObstacleSpeed& speed);
f32 GetObstacleSpeed(ObstacleSpeed const& speed);
// ===========================================================================

// Obstacle
// ---------------------------------------------------------------------------
// Represents a single hazard in the game world.
//
// Members:
//   type      - The obstacle's category (Asteroid, Spike, or Non_Obstacle).
//   position  - Current world-space center position.
//   velocity  - Movement speed (pixels/sec). Asteroids move and bounce;
//               Spikes are typically stationary.
//   half_size - Half the width/height of the bounding box. Used for
//               AABB collision detection and rendering dimensions.
// ---------------------------------------------------------------------------
class Obstacle
{
public:
	ObstacleType type{};
	AEVec2 position{};
	AEVec2 velocity{};
	AEVec2 half_size{};
	Obstacle() = default;

	// Parameterized constructor for creating obstacles from level data
	Obstacle(ObstacleType type, AEVec2 pos, AEVec2 velocity, AEVec2 half_size)
		: type{ type }, position{ pos },
		velocity{ velocity }, half_size{ half_size } {
	}

	// Reset
	// -----------------------------------------------------------------------
	// Randomizes this obstacle's position, size, and velocity for the
	// endless game mode. Spawns offscreen to the right and moves left.
	// Size is random between 25-70 pixels. Velocity is randomized
	// to create varied movement patterns.
	// -----------------------------------------------------------------------
	void Reset();

	// Update
	// -----------------------------------------------------------------------
	// Moves the obstacle based on velocity, bounces off top/bottom screen
	// edges, and handles offscreen behavior.
	//
	// Parameters:
	//   delta_time     - Frame time for frame-rate independent movement.
	//   cam_x          - Camera X position for offscreen distance checks.
	//   offscreen_limit- Distance from camera beyond which obstacle is recycled.
	//   endless        - If true: Reset() when offscreen (endless mode).
	//                    If false: mark as Non_Obstacle for deletion (custom level).
	// -----------------------------------------------------------------------
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

// ObstacleSystem
// ---------------------------------------------------------------------------
// Manages a collection of Obstacle objects using a std::vector.
// Used in custom levels where obstacles are loaded from file and may
// be dynamically added or removed during gameplay.
//
// In endless mode (Playing.cpp), a fixed-size C array is used instead
// for simplicity, but the system could be refactored to use this class.
// ---------------------------------------------------------------------------
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

// Free function wrappers (legacy interface)
void resetObstacle(Obstacle* obstacle);
//void updateObstacles(Obstacle* obstacles, f32 delta_time);

#endif // !OBSTACLE_HPP