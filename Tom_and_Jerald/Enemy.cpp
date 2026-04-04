#include "pch.hpp"
#include "Enemy.hpp"
#include "Utils.hpp"

Cat::Cat()
{
    AEVec2Zero(&m_pos);
    AEVec2Zero(&m_velocity);
}

void Cat::Init(AEVec2 player_pos)
{
    m_floor_y = AEGfxGetWinMinY() + m_half_size.y;
    m_pos.x = player_pos.x - AEGfxGetWinMaxX() - m_half_size.x;
    m_pos.y = m_floor_y;
    m_active = true;
    AEVec2Zero(&m_velocity);
}

void Cat::Update(f32 delta_time, AEVec2 player_pos)
{
    if (!m_active) return;

    f32 floor_y = AEGfxGetWinMinY() + m_half_size.y;

    //X: rubber-band chase behind player
    f32 target_x = player_pos.x;
    f32 diff_x = target_x - m_pos.x;
    f32 catchup = AEClamp(std::abs(diff_x) / k_scale_range,
        k_min_scale, k_max_scale);
    m_velocity.x = (diff_x >= 0.0f ? 1.0f : -1.0f)
        * m_chase_speed * catchup;
    m_pos.x += m_velocity.x * delta_time;

    
    //Y: jump toward player
    m_is_grounded = (m_pos.y <= floor_y + 1.0f);

    m_jump_timer -= delta_time;
    if (m_is_grounded && m_jump_timer <= 0.0f)
    {
		// Jump if player is closer than target offset
        if (diff_x > abs(k_target_offset)) {
			std::cout << "Cat decides not to jump, player is too far: diff_x=" << diff_x << "\n"; // DEBUG LINE
            m_jump_timer = m_jump_interval; // reset timer even if not jumping to prevent constant jump checks
            return;
		}
        // Scale jump force based on how high the player is
        f32 height_diff = player_pos.y - floor_y;
        // v^2 = 2 * |g| * h  ->  v = sqrt(2 * |g| * h), clamped to a min kick
        f32 needed = sqrtf(2.0f * std::abs(k_gravity) * AEMax(height_diff, 50.0f));
        m_jump_velocity = AEClamp(needed, k_jump_force * 0.5f, k_jump_force);
        m_jump_timer = m_jump_interval;
    }

    // Apply gravity
    m_jump_velocity += k_gravity * delta_time;
    m_pos.y += m_jump_velocity * delta_time;

    // Land on floor
    if (m_pos.y <= floor_y)
    {
        m_pos.y = floor_y;
        m_jump_velocity = 0.0f;
    }
}

// Returns true if the cat just killed the player this frame
bool Cat::CheckPlayerCollision(AEVec2 player_pos, AEVec2 player_half_size) const
{
    if (!m_active) return false;

    

    AEVec2 cat_pos = m_pos;           // need non-const ptr for checkOverlap
    AEVec2 cat_hs = m_half_size;
    AEVec2 p_pos = player_pos;
    AEVec2 p_hs = player_half_size;
    return checkOverlap(&p_pos, &p_hs, &cat_pos, &cat_hs);
}

void Cat::Draw(AEGfxVertexList* mesh)
{
    if (!m_active) return;

    drawQuad(mesh,
        m_pos.x, m_pos.y,
        m_half_size.x * 2.0f, m_half_size.y * 2.0f);
}

void Cat::Reset(AEVec2 player_pos)
{
    m_pos.x = player_pos.x + k_x_offset;
    m_pos.y = m_floor_y;
    AEVec2Zero(&m_velocity);
    m_active = true;
}