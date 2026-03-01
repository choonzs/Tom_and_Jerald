#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "AEEngine.h"

class PlayerConfig {
public:
    PlayerConfig() : speed(300.0f), acceleration(500.0f), max_health(3) {}
    bool LoadFromFile(const char* filename);
    f32 Speed() const { return speed; }
    f32 Acceleration() const { return acceleration; }
    int MaxHealth() const { return max_health; }
private:
    f32 speed;
    f32 acceleration;
    int max_health;
};

class Player {
public:
    Player() : health(3), texture(nullptr), mesh(nullptr) {
        AEVec2Zero(&position);
        AEVec2Zero(&velocity);
        AEVec2Set(&half_size, 20.0f, 20.0f);
    }
    ~Player();

    void Movement(f32 delta_time);

    // Getters and Setters
    AEVec2& Position() { return position; }
    const AEVec2& Position() const { return position; }

    AEVec2& Velocity() { return velocity; }
    const AEVec2& Velocity() const { return velocity; }

    AEVec2& Half_Size() { return half_size; }
    const AEVec2& Half_Size() const { return half_size; } // Const version added

    int& Health() { return health; }
    int Health() const { return health; } // Const version added

    AEGfxTexture*& Texture() { return texture; }
    AEGfxVertexList*& Mesh() { return mesh; }

    PlayerConfig& Config() { return config; }
    const PlayerConfig& Config() const { return config; } // Const version added

private:
    AEVec2 position;
    AEVec2 velocity;
    AEVec2 half_size;
    int health;
    AEGfxTexture* texture;
    AEGfxVertexList* mesh;
    PlayerConfig config;
};

#endif // !PLAYER_HPP