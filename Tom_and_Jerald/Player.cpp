#include "Player.hpp"
namespace {
	//Earth grav 9.81, testing with 5.0f
	f32 gravity = 981.f;
	f32 max_energy{};

	f32 tap_timer{ 0.0f };
	bool wait_double_tap{ false };
	bool w_released{ false };
}



Player::~Player() {
	if (mesh) AEGfxMeshFree(mesh);
	if (texture) AEGfxTextureUnload(texture);
}

void Player::Movement(f32 dt) {
	if (wait_double_tap)
	{
		tap_timer -= dt;
		if (tap_timer <= 0.0f)
		{
			wait_double_tap = false;  // too slow, reset
		}
	}

	AEVec2 added;
	// If no input by default gravity will pull the player down
	AEVec2Set(&added, 0.0f, static_cast<f32>(-gravity * dt));

	if (AEInputCheckCurr(AEVK_W)) {
		// Fwd Angle + accel * dt
		AEVec2Set(&added, 0, static_cast<f32>(k_acceleration * dt));
	}	
	if (AEInputCheckCurr(AEVK_S)) {
		AEVec2Set(&added, 0, static_cast<f32>(-k_acceleration * dt));
	}

	if (AEInputCheckTriggered(AEVK_W) && w_released) {
		// If the player double taps W, give them a boost
		if (wait_double_tap) {
			float burstStrength = 500.0f;

			AEVec2 burst;
			AEVec2Set(&burst, 0.0f, burstStrength);

			AEVec2Add(&velocity, &velocity, &burst);

			wait_double_tap = false;
			boost_timer = 3.0f; // 3 seconds cooldown
		}
		wait_double_tap = true;
		tap_timer = 0.5f; // 0.5 seconds to double tap
		w_released = false;
	}

	// Recharge boost if timer is up
	if (boost_timer > 0.0f) {
		boost_timer -= dt;
		if (boost_timer <= 0.0f) {
			boost_rocket = true;
		}
	}
	
	if (AEInputCheckReleased(AEVK_W)) {
		w_released = true;
	}
	
	//Get new velocity based on accel and current vel
	// Find the velocity according to the acceleration
	AEVec2 newVel;
	AEVec2Add(&newVel, &added, &velocity);
	AEVec2Scale(&newVel, &newVel, 0.99f);

	// Set current velocity to new velocity
	AEVec2Set(&velocity, newVel.x, newVel.y);

	AEVec2Set(&position, f32(position.x + velocity.x * dt), f32(position.y + velocity.y * dt));
	


	f32 min_x = AEGfxGetWinMinX() + Half_Size().x;
	f32 max_x = AEGfxGetWinMaxX() - Half_Size().x;
	f32 min_y = AEGfxGetWinMinY() + Half_Size().y;
	f32 max_y = AEGfxGetWinMaxY() - Half_Size().y;


	if (Position().x < min_x)
		Position().x = min_x;
	if (Position().x > max_x)
		Position().x = max_x;
	if (Position().y < min_y)
		Position().y = min_y;
	if (Position().y > max_y) {
		Position().y = max_y;
	}
}


