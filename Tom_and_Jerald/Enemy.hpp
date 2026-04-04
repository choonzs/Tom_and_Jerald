/*************************************************************************
@file Enemy.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Declares the Cat class, a chasing enemy that pursues the player across
    the ground with a rubber-band speed system and periodic jumps.

    Chase behaviour:
    The cat targets a position slightly behind the player (k_target_offset).
    Its X speed scales with the gap between them: the further behind it
    falls, the faster it closes in (rubber-band effect clamped between
    k_min_scale and k_max_scale multipliers).

    Jump behaviour:
    The cat jumps every m_jump_interval seconds while grounded, but only
    if the player is close enough (within k_target_offset distance). Jump
    force is derived from the player's height above the floor so the cat
    can reach them on elevated platforms.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "pch.hpp"

class Cat
{
public:
    Cat();
    ~Cat() = default;

    // Places the cat offscreen-left relative to the player and resets state.
    void Init(AEVec2 player_pos);

    // Advances chase and jump logic by one frame.
    void Update(f32 delta_time, AEVec2 player_pos);

    // Draws the cat as a quad using the provided mesh.
    void Draw(AEGfxVertexList* mesh) const;

    // Repositions the cat behind the player and reactivates it.
    void Reset(AEVec2 player_pos);

    // Returns true if the cat's AABB overlaps the player's AABB this frame.
    bool CheckPlayerCollision(AEVec2 player_pos, AEVec2 player_half_size) const;

    // Accessors
    AEVec2& Position() { return m_pos; }
    AEVec2  Position()  const { return m_pos; }

    AEVec2& HalfSize() { return m_half_size; }
    AEVec2  HalfSize()  const { return m_half_size; }

    bool    IsActive()  const { return m_active; }
    void    SetActive(bool v) { m_active = v; }

    f32     FloorY()    const { return m_floor_y; }

private:
    // -----------------------------------------------------------------------
    // Transform & physics
    // -----------------------------------------------------------------------
    AEVec2 m_pos{};
    AEVec2 m_half_size{ 100.0f, 100.0f };
    AEVec2 m_velocity{};

    f32    m_floor_y{};           // Computed on Init from window bounds + offset.
    f32    m_jump_velocity = 0.0f;
    bool   m_is_grounded = false;

    // -----------------------------------------------------------------------
    // Chase & jump timing
    // -----------------------------------------------------------------------
    f32    m_chase_speed = 120.0f;
    f32    m_jump_timer = 0.0f;
    f32    m_jump_interval = 2.0f;   // Seconds between jump attempts.
    bool   m_active = true;

    // -----------------------------------------------------------------------
    // Tuning constants
    // -----------------------------------------------------------------------
    static constexpr f32 k_floor_offset = 50.0f;   // Gap between window bottom and ground.
    static constexpr f32 k_x_offset = -200.0f; // Spawn offset behind the player.
    static constexpr f32 k_target_offset = -150.0f; // Desired X gap when chasing.
    static constexpr f32 k_min_scale = 1.0f;    // Minimum chase speed multiplier.
    static constexpr f32 k_max_scale = 3.0f;    // Maximum chase speed multiplier.
    static constexpr f32 k_scale_range = 200.0f;  // Distance over which speed ramps up.
    static constexpr f32 k_gravity = -800.0f; // Downward acceleration (px/s²).
    static constexpr f32 k_jump_force = 600.0f;  // Maximum upward jump velocity (px/s).
};

#endif // ENEMY_HPP