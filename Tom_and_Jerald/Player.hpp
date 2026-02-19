#include "pch.hpp"
#ifndef PLAYER_HPP 
#define PLAYER_HPP
namespace {
	// Temp placement of stuff until asiohjfioheiohodeiqj
	const int k_max_health = 10;
	const int k_obstacle_count = 10;
	const f32 k_stage_duration = 30.0f;
	const f32 k_player_speed = 500.0f;
	const f32 k_damage_cooldown = 0.4f;
	const f32 k_player_base_half_size = 40.0f;

	const f32 k_acceleration = 500.0f;
}



class Player {
	public:	
		//ctor
		Player() : position{ 0.0f, 0.0f }, prev_position{ position }, 
			velocity{ 0.0f, 0.0f },
			half_size{ 40.0f, 40.0f }, health(k_max_health), 
			texture(nullptr), mesh(nullptr) {}
		//dtor
		~Player();


		// accessors
		AEVec2 Position() const { return position; }
		AEVec2 Half_Size() const { return half_size; }
		int Health() const { return health; }
		AEGfxTexture* Texture() const { return texture; }
		AEGfxVertexList* Mesh() const { return mesh; }


		// mutators
		AEVec2& Position() { return position; }
		AEVec2& Half_Size() { return half_size; }
		int& Health() { return health; }
		AEGfxTexture*& Texture() { return texture; }
		AEGfxVertexList*& Mesh() { return mesh; }


		// Movement
		void Movement(f32 delta_time);

	private: 
		AEVec2 position;
		AEVec2 prev_position;	// object previous position -> it's the position calculated in the previous loop
		AEVec2 velocity;	// object current velocity
		
		AEVec2 half_size;
		int health;
		AEGfxTexture* texture;
		AEGfxVertexList* mesh;

		bool boost_rocket{ true };
		f32 boost_timer{};

};


#endif // !PLAYER_HPP