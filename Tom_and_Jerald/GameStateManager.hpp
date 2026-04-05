/*************************************************************************
@file GameStateManager.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
     Declares the Game State Manager (GSM), which drives the application loop
     by tracking the current, previous, and next game states and dispatching
     lifecycle function pointers for each state.

     Each game state registers six callbacks:
       fpLoad       - Called once when first entering a state. Loads assets.
       fpInitialize - Called each time a state starts (including restarts).
       fpUpdate     - Called every frame while the state is active.
       fpDraw       - Called every frame to render the state.
       fpFree       - Called each time a state ends. Frees runtime objects.
       fpUnload     - Called once when leaving a state permanently. Unloads assets.

     State transitions:
       Set `next` to any GameState value to trigger a transition at the end of
       the current frame. Setting `next` to GAME_STATE_RESTART re-runs
       fpFree + fpInitialize without unloading assets. Setting it to
       GAME_STATE_QUIT exits the application loop.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef GAME_STATE_MANAGER_HPP
#define GAME_STATE_MANAGER_HPP

// Function pointer type for all state lifecycle callbacks.
typedef void(*FP)(void);

// Active state tracking. `previous` is set automatically by GSM_Update()
// after each transition; do not write to it directly.
extern int current;
extern int previous;
extern int next;
extern bool gSkipSplash;

// Lifecycle callbacks for the active state. Assigned by GSM_Update()
// via the state table; do not assign directly.
extern FP fpLoad;
extern FP fpInitialize;
extern FP fpUpdate;
extern FP fpDraw;
extern FP fpFree;
extern FP fpUnload;

// Initializes the GSM with the given starting state and populates the
// function pointers ready for the first iteration of the application loop.
void GSM_Initialize(int startingState);

// Called once per frame by the application loop. Executes the current
// state's update and draw callbacks, then handles any pending transition
// by calling the appropriate free/unload/load/initialize sequence.
void GSM_Update();

#endif // GAME_STATE_MANAGER_HPP