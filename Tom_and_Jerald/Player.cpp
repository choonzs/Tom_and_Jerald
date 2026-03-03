#include "pch.hpp"
#include "Player.hpp"



Player::~Player() {
    // Empty Destructor
    if (mesh) AEGfxMeshFree(mesh);
	mesh = nullptr;
}

// Import player configurations from file
bool PlayerConfig::LoadFromFile(const char* filename) {
    std::ifstream ifs(filename);

    if (!ifs) { return false; }
    std::string tmp;

	// The file is expected to be in the format:
    ifs >> tmp;
    ifs >> speed;
    ifs >> tmp;
    ifs >> acceleration;
    ifs >> tmp;
    ifs >> max_health;

    return true;
}


void Player::Movement(f32 dt) {
    
    AEVec2 added{};
    // If no input by default gravity will pull the player down
    AEVec2Set(&added, 0.0f, 0.0f);

    // 1. VERTICAL MOVEMENT (Jetpack / Gravity)
    if (AEInputCheckCurr(AEVK_SPACE) || AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_UP)) {
		added.y += static_cast<f32>(Config().Acceleration() * dt);
    }
    else {
        added.y += -static_cast<f32>(Config().Acceleration() * dt);
    }
    
    // 2. HORIZONTAL MOVEMENT (True Side Scroller)
    if (AEInputCheckCurr(AEVK_D) || AEInputCheckCurr(AEVK_RIGHT)) {
		added.x = static_cast<f32>(Config().Acceleration() * dt);
    }
    else if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
        added.x = -static_cast<f32>(Config().Acceleration() * dt * 0.5f);
        if (velocity.x < static_cast<f32>(Config().Acceleration() * dt)) {
            added.x = static_cast<f32>(Config().Speed() * dt); // No horizontal input, no horizontal acceleration
        }
    }
    else {
		added.x = static_cast<f32>(Config().Speed() * dt); // No horizontal input, no horizontal acceleration
    }

    // Find the velocity according to the acceleration
    AEVec2 newVel;
    AEVec2Add(&newVel, &added, &velocity);
	// Apply simple air resistance to slow down the player when not accelerating
    AEVec2Scale(&newVel, &newVel, 0.99f);
    // Set current velocity to new velocity
    AEVec2Set(&velocity, newVel.x, newVel.y);

    // 3. APPLY VELOCITY TO POSITION
    AEVec2Set(&position, f32(position.x + velocity.x * dt), f32(position.y + velocity.y * dt));
    
    // 4. PREVENT FALLING INTO THE VOID
    f32 bottomBoundary = AEGfxGetWinMinY() + half_size.y;
    if (position.y < bottomBoundary) {
        position.y = bottomBoundary;
        velocity.y = 0.0f;
    }
	// 5. PREVENT GOING ABOVE THE CEILING
    f32 topBoundary = AEGfxGetWinMaxY() - half_size.y;
    if (position.y > topBoundary) {
        position.y = topBoundary;
        velocity.y = 0.0f;
	}
}