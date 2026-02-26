#ifndef VICTORY_HPP
#define VICTORY_HPP

#include "pch.hpp"

// The GameStateManager looks for these specific C-style global functions.
// They must be declared here so they can be "seen" by GameStateManager.cpp.

void Victory_Load();
void Victory_Initialize();
void Victory_Update();
void Victory_Draw();
void Victory_Free();
void Victory_Unload();

class VictoryState {
public:
    void Load();
    void Initialize();
    void Update();
    void Draw();
    void Free();
    void Unload();

private:
    s8 font_id = -1;
    AEGfxVertexList* unit_square = nullptr;
};

#endif // !VICTORY_HPP