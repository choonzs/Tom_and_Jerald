/*************************************************************************
@file Enemy.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Implements Cat: a ground-based chasing enemy with rubber-band X speed
    and physics-derived jumps.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "Enemy.hpp"
#include "Utils.hpp"

// ---------------------------------------------------------------------------
// Cat::Cat
// ---------------------------------------------------------------------------
// Zero-initializes position and velocity. Member initializers in the header
// handle all other defaults.
// ---------------------------------------------------------------------------
Cat::Cat()
{
    AEVec2Zero(&m_pos);
    AEVec2Zero(&m_velocity);
}

// ---------------------------------------------------------------------------
// Cat::Init
// ---------------------------------------------------------------------------
// Computes the floor Y from window bounds, places the cat offscreen-left
// relative to the player, and resets all physics state.
// ---------------------------------------------------------------------------
void Cat::Init(AEVec2 player_pos)
{
    m_floor_y = AEGfxGetWinMinY() + m_half_size.y + k_floor_offset;

    // Start fully offscreen to the left so it scrolls naturally into view.
    m_pos.x = player_pos.x - AEGfxGetWinMaxX() - m_half_size.x;
    m_pos.y = m_floor_y;

    m_active = true;
    m_jump_velocity = 0.0f;
    m_jump_timer = 0.0f;
    AEVec2Zero(&m_velocity);
}

// ---------------------------------------------------------------------------
// Cat::Update
// ---------------------------------------------------------------------------
// Per-frame movement: rubber-band X chase, then Y gravity and jump logic.
//
// X (rubber-band chase):
//   Speed scales with distance to the player, clamped between k_min_scale
//   and k_max_scale, so the cat catches up when far but doesn't overshoot
//   wildly when close.
//
// Y (jump):
//   While grounded and the timer has expired, the cat jumps only if the
//   player is within k_target_offset distance. Jump velocity is derived
//   from the kinematic formula v = sqrt(2 * |g| * h) so the cat can
//   reach the player's current height, clamped to [50%, 100%] of k_jump_force.
// ---------------------------------------------------------------------------
void Cat::Update(f32 delta_time, AEVec2 player_pos)
{
    if (!m_active) return;

    const f32 floor_y = AEGfxGetWinMinY() + m_half_size.y + k_floor_offset;

    // --- X: rubber-band chase ---
    const f32 diff_x = player_pos.x - m_pos.x;
    const f32 catchup = AEClamp(std::abs(diff_x) / k_scale_range,
        k_min_scale, k_max_scale);

    m_velocity.x = (diff_x >= 0.0f ? 1.0f : -1.0f) * m_chase_speed * catchup;
    m_pos.x += m_velocity.x * delta_time;

    // --- Y: jump logic ---
    m_is_grounded = (m_pos.y <= floor_y + 1.0f);
    m_jump_timer -= delta_time;

    if (m_is_grounded && m_jump_timer <= 0.0f)
    {
        m_jump_timer = m_jump_interval; // Reset regardless of whether we jump.

        // Only jump if the player is close enough to be worth it.
        if (diff_x <= std::abs(k_target_offset))
        {
            // Derive jump velocity from the player's height above the floor.
            // Kinematic: v = sqrt(2 * |g| * h), with a minimum kick of 50% force.
            const f32 height_diff = player_pos.y - floor_y;
            const f32 needed = sqrtf(2.0f * std::abs(k_gravity)
                * AEMax(height_diff, 50.0f));
            m_jump_velocity = AEClamp(needed, k_jump_force * 0.5f, k_jump_force);
        }
    }

    // Apply gravity and integrate Y position.
    m_jump_velocity += k_gravity * delta_time;
    m_pos.y += m_jump_velocity * delta_time;

    // Land on floor.
    if (m_pos.y <= floor_y)
    {
        m_pos.y = floor_y;
        m_jump_velocity = 0.0f;
    }

    // If cat is too far offscreen to the left, snap it back just off left edge
    const f32 cam_left = player_pos.x - (AEGfxGetWinMaxX() - AEGfxGetWinMinX()) * 0.5f;
    const f32 too_far_threshold = cam_left - 200.0f; // 200px past left screen edge

    if (m_pos.x < too_far_threshold)
    {
        // Place just off the left edge of the screen, not visible to player
        m_pos.x = cam_left - m_half_size.x - 10.0f;
        m_pos.y = floor_y;
        m_jump_velocity = 0.0f;
    }
}

// ---------------------------------------------------------------------------
// Cat::CheckPlayerCollision
// ---------------------------------------------------------------------------
// Returns true if the cat's AABB overlaps the player's AABB.
// Local copies are required because checkOverlap takes non-const pointers.
// ---------------------------------------------------------------------------
bool Cat::CheckPlayerCollision(AEVec2 player_pos, AEVec2 player_half_size) const
{
    if (!m_active) return false;

    AEVec2 cat_pos = m_pos;
    AEVec2 cat_hs = m_half_size;
    AEVec2 p_pos = player_pos;
    AEVec2 p_hs = player_half_size;
    return checkOverlap(&p_pos, &p_hs, &cat_pos, &cat_hs);
}

// ---------------------------------------------------------------------------
// Cat::Draw
// ---------------------------------------------------------------------------
// Renders the cat as a full-size quad (half_size * 2) centered on m_pos.
// ---------------------------------------------------------------------------
void Cat::Draw(AEGfxVertexList* mesh) const
{
    if (!m_active) return;

    drawQuad(mesh,
        m_pos.x, m_pos.y,
        m_half_size.x * 2.0f, m_half_size.y * 2.0f);
}

// ---------------------------------------------------------------------------
// Cat::Reset
// ---------------------------------------------------------------------------
// Repositions the cat just behind the player at floor level and reactivates
// it. Used when restarting a round without a full reinitialisation.
// ---------------------------------------------------------------------------
void Cat::Reset(AEVec2 player_pos)
{
    m_pos.x = player_pos.x + k_x_offset;
    m_pos.y = m_floor_y;
    m_jump_velocity = 0.0f;
    m_active = true;
    AEVec2Zero(&m_velocity);
}