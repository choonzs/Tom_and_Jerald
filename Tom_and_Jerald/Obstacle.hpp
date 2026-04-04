/*************************************************************************
@file Obstacle.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu
@brief  
    Declares the Obstacle struct, ObstacleSystem manager, and supporting 
    enums for obstacle type, scale, and speed.

 Obstacle types:
   Non_Obstacle  - Marked for deletion. Set when an obstacle goes offscreen
                   in non-endless (custom level) mode.
   Asteroid      - Moves and bounces off screen top/bottom edges.
   Spike         - Static hazard; spawns on ceiling or floor.
   Wall          - Behaves identically to Spike (shares spawn logic).

 ObstacleSystem:
   Wraps a std::vector<Obstacle> for use in custom levels where obstacles
   are loaded from file and managed dynamically. Endless mode uses a
   fixed-size array directly in Playing.cpp instead.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

// Obstacle.hpp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "pch.hpp"

// ---------------------------------------------------------------------------
// ObstacleType
// ---------------------------------------------------------------------------
enum ObstacleType : int
{
    Non_Obstacle = 0,
    Asteroid = 1,
    Spike = 2,
    Wall = 3
};

// ---------------------------------------------------------------------------
// ObstacleScale
// Represents discrete size tiers for obstacles. Increment/decrement
// operators wrap around at the enum boundaries.
// ---------------------------------------------------------------------------
enum class ObstacleScale : int
{
    Tiny = 0,
    Small,
    Normal,
    Large,
    Giant
};

ObstacleScale& operator--(ObstacleScale& scale);  // Wraps Giant -> Tiny
ObstacleScale& operator++(ObstacleScale& scale);  // Wraps Tiny  -> Giant

// Returns the pixel size multiplier for the given scale tier.
f32 GetObstacleSize(ObstacleScale const& scale);

// ---------------------------------------------------------------------------
// ObstacleSpeed
// Represents discrete speed tiers for obstacles. Speeds are negative
// (obstacles always move left toward the player).
// ---------------------------------------------------------------------------
enum class ObstacleSpeed : int
{
    Slow = 0,
    Normal,
    Fast
};

ObstacleSpeed& operator--(ObstacleSpeed& speed);  // Wraps Slow -> Fast
ObstacleSpeed& operator++(ObstacleSpeed& speed);  // Wraps Fast -> Slow

// Returns the X velocity (negative = leftward) for the given speed tier.
f32 GetObstacleSpeed(ObstacleSpeed const& speed);

// ===========================================================================
// Obstacle
// ---------------------------------------------------------------------------
// Represents a single hazard in the game world.
//
// Members:
//   type      - Category (Asteroid, Spike, Wall, or Non_Obstacle).
//   position  - World-space center position.
//   velocity  - Movement in pixels/sec. Asteroids move and bounce;
//               Spikes and Walls are stationary (velocity = 0).
//   half_size - Half the bounding box dimensions, used for both
//               AABB collision detection and rendering.
//   rotation  - Rotation in radians. Used to orient Spikes/Walls
//               on the ceiling (PI) vs. floor (0).
// ===========================================================================
class Obstacle
{
public:
    ObstacleType type{};
    AEVec2       position{};
    AEVec2       velocity{};
    AEVec2       half_size{};
    f32          rotation{};

    // Default constructor: randomizes all fields via Reset().
    Obstacle() { Reset(); }

    // Parameterized constructor: initializes from explicit values.
    // Used when loading obstacles from a level file.
    Obstacle(ObstacleType type, AEVec2 pos, AEVec2 velocity, AEVec2 half_size)
        : type{ type }, position{ pos },
        velocity{ velocity }, half_size{ half_size } {
    }

    // Reset
    // -----------------------------------------------------------------------
    // Randomizes this obstacle for reuse in endless mode. Called on
    // construction and whenever the obstacle scrolls offscreen.
    //
    // Asteroids spawn offscreen-right with random size, X/Y velocity.
    // Spikes and Walls spawn on either the ceiling or floor with no velocity.
    // -----------------------------------------------------------------------
    void Reset();

    // Update
    // -----------------------------------------------------------------------
    // Moves the obstacle, bounces it off screen edges, and handles offscreen
    // recycling or deletion.
    //
    // Parameters:
    //   delta_time      - Frame time in seconds.
    //   cam_x           - Camera X used to calculate offscreen distance.
    //   offscreen_limit - Distance past camera at which obstacle is recycled.
    //   endless         - true  = Reset() when offscreen (endless mode).
    //                     false = Set type to Non_Obstacle for deletion
    //                             (custom level mode).
    // -----------------------------------------------------------------------
    void Update(f32 delta_time, f32 cam_x, f32 offscreen_limit, bool endless);

    // Accessors
    ObstacleType& Type() { return type; }
    ObstacleType const& Type()  const { return type; }

    AEVec2& Position() { return position; }
    AEVec2 const& Position()    const { return position; }
    AEVec2 const* PositionPtr() const { return &position; }

    AEVec2& Velocity() { return velocity; }
    AEVec2 const& Velocity()    const { return velocity; }
    AEVec2 const* VelocityPtr() const { return &velocity; }

    AEVec2& HalfSize() { return half_size; }
    AEVec2 const& HalfSize()    const { return half_size; }
    AEVec2 const* HalfSizePtr() const { return &half_size; }
};

// ===========================================================================
// ObstacleSystem
// ---------------------------------------------------------------------------
// Manages a dynamic list of obstacles for custom (non-endless) levels.
// Obstacles are loaded from file and stored in a std::vector. The vector
// can be compacted externally by erasing entries whose type == Non_Obstacle.
// ===========================================================================
class ObstacleSystem
{
public:
    ObstacleSystem() : obstacles(0) {}

    // Null-safe wrapper around Obstacle::Reset().
    static void ResetObstacle(Obstacle* obstacle);

    // Appends an obstacle to the managed list.
    void AddObstacle(Obstacle& obstacle) { obstacles.push_back(obstacle); }

    // Accessors
    std::vector<Obstacle>& Obstacles() { return obstacles; }
    const std::vector<Obstacle>& Obstacles() const { return obstacles; }

private:
    std::vector<Obstacle> obstacles;
};

// Legacy free-function wrapper kept for backward compatibility.
void resetObstacle(Obstacle* obstacle);

#endif // OBSTACLE_HPP
