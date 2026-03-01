#include "pch.hpp"
#include "Player.hpp"

Player::~Player() {
    // Free resources if necessary
}

bool PlayerConfig::LoadFromFile(const char* filename) {
    // Keep your existing config logic
    return true;
}

void Player::Movement(f32 delta_time) {
    // 1. VERTICAL MOVEMENT (Jetpack / Gravity)
    if (AEInputCheckCurr(AEVK_SPACE) || AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_UP)) {
        velocity.y = config.Speed(); // Fly upwards
    }
    else {
        velocity.y -= config.Acceleration() * delta_time; // Gravity pulls down
    }

    // 2. HORIZONTAL MOVEMENT (True Side Scroller)
    if (AEInputCheckCurr(AEVK_D) || AEInputCheckCurr(AEVK_RIGHT)) {
        velocity.x = config.Speed(); // Move Right
    }
    else if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
        velocity.x = -config.Speed(); // Move Left
    }
    else {
        velocity.x = 0.0f; // Stop moving horizontally when no keys are pressed
    }

    // 3. APPLY VELOCITY TO POSITION
    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;

    // 4. PREVENT FALLING INTO THE VOID
    // Adds a safety floor at the bottom of the camera so the player doesn't fall infinitely
    f32 bottomBoundary = AEGfxGetWinMinY() + half_size.y;
    if (position.y < bottomBoundary) {
        position.y = bottomBoundary;
        velocity.y = 0.0f;
    }
}