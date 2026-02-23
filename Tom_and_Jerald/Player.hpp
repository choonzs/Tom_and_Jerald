#include "pch.hpp"
#ifndef PLAYER_HPP 
#define PLAYER_HPP
namespace {
	// Temp placement of stuff until asiohjfioheiohodeiqj
	const int k_obstacle_count = 10;
	const f32 k_stage_duration = 30.0f;
	const f32 k_damage_cooldown = 1.0f;


	const int k_max_health = 10; //player
	const f32 k_player_speed = 500.0f;
	const f32 k_player_base_half_size = 40.0f;

}
class PlayerConfig {
public:
	bool LoadFromFile(const char* filename);

	f32 Speed() const { return speed; }
	f32 Acceleration() const { return acceleration; }
	int MaxHealth() const { return maxHealth; }

private:
	f32 speed = 500.0f;        // default values
	f32 acceleration = 500.0f;
	int maxHealth = 10;
};



class Player {
	public:	
		//ctor
		Player() : position{ 0.0f, 0.0f }, prev_position{ position }, 
			velocity{ 0.0f, 0.0f },
			half_size{ 40.0f, 40.0f }, health(10), 
			texture(nullptr), mesh(nullptr) {
			config.LoadFromFile("PlayerConfig.txt");
		}
		//dtor
		~Player();


		// accessors
		AEVec2 Position() const { return position; }
		AEVec2 Half_Size() const { return half_size; }
		int Health() const { return health; }
		AEGfxTexture* Texture() const { return texture; }
		AEGfxVertexList* Mesh() const { return mesh; }

		PlayerConfig Config() const { return config; }


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

		PlayerConfig config;

};


#endif // !PLAYER_HPP