/*************************************************************************
@file Credits.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu
@brief
      Implements the in-game currency system ("Cheese").

     Players earn cheese passively: every k_credit_interval seconds without
     taking damage awards k_credit_reward cheese. Taking damage resets the
     timer, incentivising careful play. Cheese is spent in the Shop on
     upgrades (health, size, fuel capacity, etc.).

     Persistence:
       The balance is held in a file-scope static. It survives state transitions
       within a session but resets to 0 on a fresh launch. Use Credits_SaveFile
       and Credits_LoadFile to persist it across runs.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#include "pch.hpp"
#include "Credits.hpp"

// ---------------------------------------------------------------------------
// File-scope state
// ---------------------------------------------------------------------------

namespace
{
    int credits = 0;    // Current cheese balance.
    int credits_init = 0;    // Balance at last load/save (used to detect unsaved changes).
    f32 no_damage_timer = 0.0f; // Seconds since the player last took damage.
}

// Cheese earned in the current round. Exposed for HUD/score display.
int credits_this_round = 0;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void Credits_ResetRound()
{
    no_damage_timer = 0.0f;
}

// ---------------------------------------------------------------------------
// Credits_Update
// ---------------------------------------------------------------------------
// Accumulates the no-damage timer and awards cheese for each full interval
// elapsed. The while loop handles frames where delta_time exceeds the
// interval (e.g. after a hitch), awarding multiple rewards correctly
// rather than skipping them.
// ---------------------------------------------------------------------------
void Credits_Update(f32 delta_time, bool game_active)
{
    if (!game_active)
        return;

    no_damage_timer += delta_time;

    while (no_damage_timer >= k_credit_interval)
    {
        credits += k_credit_reward;
        no_damage_timer -= k_credit_interval;
    }
}

void Credits_OnDamage()
{
    no_damage_timer = 0.0f;
}

// ---------------------------------------------------------------------------
// Balance queries and mutations
// ---------------------------------------------------------------------------

int Credits_GetBalance()
{
    return credits;
}

int Credits_GetInitBalance()
{
    return credits_init;
}

bool Credits_CanAfford(int cost)
{
    return credits >= cost;
}

bool Credits_Spend(int cost)
{
    if (!Credits_CanAfford(cost))
        return false;
    credits -= cost;
    return true;
}

void Credits_Add(int amount)
{
    if (amount > 0)
        credits += amount;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

void Credits_LoadFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
        return; // File absent: balance stays at 0.

    ifs >> credits;
    if (!ifs)
        credits = 0; // File empty or corrupt: fall back to 0.

    credits_init = credits;
}

void Credits_SaveFile(const std::string& filename)
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
        return;

    ofs << credits;
    credits_init = credits; // Mark session as clean.
}