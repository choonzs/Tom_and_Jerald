#include "pch.hpp"
#include "Player.hpp"


Player::~Player() {
    // Empty Destructor
    if (mesh) AEGfxMeshFree(mesh);
    if (texture) AEGfxTextureUnload(texture);
}

// Import player configurations from file
bool PlayerConfig::LoadFromFile(const char* filename) {
    std::ifstream ifs(filename);

    if (!ifs) { return false; }
    std::string tmp;

    ifs >> tmp;
    ifs >> speed;
    ifs >> tmp;
    ifs >> acceleration;
    ifs >> tmp;
    ifs >> max_health;

    return true;
}


void Player::Movement(f32 delta_time) {
    // 1. VERTICAL MOVEMENT (Jetpack / Gravity)
    if (AEInputCheckCurr(AEVK_SPACE) || AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_UP)) {
        velocity.y = config.Speed();
    }
    else {
        velocity.y -= config.Acceleration() * delta_time;
    }

    // 2. HORIZONTAL MOVEMENT (True Side Scroller)
    if (AEInputCheckCurr(AEVK_D) || AEInputCheckCurr(AEVK_RIGHT)) {
        velocity.x = config.Speed();
    }
    else if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT)) {
        velocity.x = -config.Speed();
    }
    else {
        velocity.x = 0.0f;
    }

    // 3. APPLY VELOCITY TO POSITION
    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;

    // 4. PREVENT FALLING INTO THE VOID
    f32 bottomBoundary = AEGfxGetWinMinY() + half_size.y;
    if (position.y < bottomBoundary) {
        position.y = bottomBoundary;
        velocity.y = 0.0f;
    }
}