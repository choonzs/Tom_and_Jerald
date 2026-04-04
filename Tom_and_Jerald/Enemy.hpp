#include "pch.hpp"


class Cat {
	public:
		Cat();
		~Cat() = default;

		void Init(AEVec2 player_pos);
		void Update(f32 delta_time, AEVec2 player_pos);
		void Draw(AEGfxVertexList* mesh);
		void Reset(AEVec2 player_pos);

		// Accessors
		AEVec2& Position() { return m_pos; }
		AEVec2 Position() const { return m_pos; }	
		AEVec2& Half_Size() { return m_half_size; }
		AEVec2 Half_Size() const { return m_half_size; }
		bool    IsActive()  const { return m_active; }
		void    SetActive(bool v) { m_active = v; }

		f32 FloorY() const { return m_floor_y; }

		bool CheckPlayerCollision(AEVec2 player_pos, AEVec2 player_half_size) const;

	private:
		AEVec2 m_pos{};
		AEVec2 m_half_size{ 100.0f, 100.0f };
		AEVec2 m_velocity{};

		f32  m_chase_speed = 120.0f;
		f32  m_floor_y = 0.0f;     // set on Init from window bounds
		bool m_active = true;

		static constexpr f32 k_x_offset = -200.0f; // start behind player
		static constexpr f32 k_target_offset = -150.0f; // chases to this gap
		static constexpr f32 k_min_scale = 1.0f;
		static constexpr f32 k_max_scale = 3.0f;
		static constexpr f32 k_scale_range = 200.0f; // distance for rubber-band ramp

		f32  m_jump_velocity = 0.0f;
		f32  m_jump_timer = 0.0f;
		f32  m_jump_interval = 2.0f;  // jumps every 2secs
		bool m_is_grounded = false;

		static constexpr f32 k_gravity = -800.0f;
		static constexpr f32 k_jump_force = 600.0f;  


};