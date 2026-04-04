/*************************************************************************
@file Credits.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu
@brief
     Interface for the in-game currency system ("Cheese").

     Cheese is earned passively during gameplay: every k_credit_interval
     seconds without taking damage awards k_credit_reward cheese. Taking
     damage resets the timer. Cheese is spent in the Shop on upgrades.

     Balance persists across rounds within a session but is not kept in
     memory between application runs — use Credits_SaveFile / Credits_LoadFile
     to persist it to disk.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef CREDITS_HPP
#define CREDITS_HPP

#include "pch.hpp"

// ---------------------------------------------------------------------------
// Tuning constants
// ---------------------------------------------------------------------------

// Cheese awarded per uninterrupted interval.
inline constexpr int k_credit_reward = 10;

// Seconds of continuous play without damage required to earn a reward.
inline constexpr f32 k_credit_interval = 2.0f;

// ---------------------------------------------------------------------------
// Round tracking
// ---------------------------------------------------------------------------

// Cheese earned in the current round. Exposed for HUD/score display.
extern int credits_this_round;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

// Resets the no-damage timer at the start of a round.
// Does NOT reset the cheese balance (it persists between rounds).
void Credits_ResetRound();

// Called every frame during active gameplay to accumulate cheese.
// Pass game_active = false when paused, on game over, or in victory screen.
void Credits_Update(f32 delta_time, bool game_active);

// Called when the player takes damage. Resets the no-damage timer,
// delaying the next cheese reward.
void Credits_OnDamage();

// ---------------------------------------------------------------------------
// Balance queries and mutations
// ---------------------------------------------------------------------------

// Returns the current cheese balance (for display or logic checks).
int  Credits_GetBalance();

// Returns the balance as it was at the start of the current session
// (i.e. the value last loaded from or saved to disk). Useful for
// detecting unsaved changes.
int  Credits_GetInitBalance();

// Returns true if the current balance is >= cost.
bool Credits_CanAfford(int cost);

// Deducts cost from the balance and returns true. Returns false without
// modifying the balance if the player cannot afford it.
bool Credits_Spend(int cost);

// Adds amount to the balance. Ignores non-positive values.
void Credits_Add(int amount);

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

// Loads the cheese balance from a plain-text file. If the file is missing
// or unreadable the balance stays at 0. Also updates the init balance.
void Credits_LoadFile(const std::string& filename);

// Writes the current cheese balance to a plain-text file.
// Also updates the init balance to match, marking the session as clean.
void Credits_SaveFile(const std::string& filename);

#endif // CREDITS_HPP