// Credits.cpp
// ===========================================================================
// Implements the in-game currency system ("Cheese").
// Players earn cheese passively over time during gameplay, with earnings
// interrupted when they take damage. Cheese is spent in the Shop to
// purchase upgrades (health, size, fuel capacity, etc.).
//
// Earning Mechanic:
//   - Every k_credit_interval seconds (2.0s) of continuous play without
//     taking damage, the player earns k_credit_reward (10) cheese.
//   - Taking damage resets the no-damage timer, punishing reckless play.
//   - This creates an incentive to play carefully and dodge obstacles.
//
// Persistence:
//   The credit balance persists across game states within a session
//   (static variable), but resets when the application is closed.
// ===========================================================================

#include "pch.hpp"
#include "Credits.hpp"

namespace {
    int credits{};// Starting cheese balance
    int credits_init{credits};
    f32 no_damage_timer = 0.0f;  // Time since last damage (seconds)
}
 int credits_this_round = 0;

// ---------------------------------------------------------------------------
// Credits_ResetRound
// ---------------------------------------------------------------------------
// Resets the no-damage timer at the start of each round/game session.
// Does NOT reset the credit balance (cheese persists between rounds).
// ---------------------------------------------------------------------------
void Credits_ResetRound()
{
    no_damage_timer = 0.0f;
}

// ---------------------------------------------------------------------------
// Credits_Update
// ---------------------------------------------------------------------------
// Called each frame during active gameplay to accumulate cheese.
// Uses a "while" loop to handle edge cases where delta_time exceeds
// the credit interval (e.g., if the game freezes briefly).
//
// Parameters:
//   delta_time  - Seconds since last frame.
//   game_active - Only accumulate when the game is actively running
//                 (not paused, not game over, not in victory screen).
// ---------------------------------------------------------------------------
void Credits_Update(f32 delta_time, bool game_active)
{
    if (!game_active)
        return;

    no_damage_timer += delta_time;
    // Award credits for each full interval elapsed
    while (no_damage_timer >= k_credit_interval)
    {
        credits += k_credit_reward;
        no_damage_timer -= k_credit_interval;
    }
}

// ---------------------------------------------------------------------------
// Credits_OnDamage
// ---------------------------------------------------------------------------
// Called when the player takes damage. Resets the no-damage timer,
// effectively punishing the player by delaying their next cheese reward.
// ---------------------------------------------------------------------------
void Credits_OnDamage()
{
    no_damage_timer = 0.0f;
}

// ---------------------------------------------------------------------------
// Credits_GetBalance / Credits_CanAfford / Credits_Spend / Credits_Add
// ---------------------------------------------------------------------------
// Standard currency operations used by the Shop:
//   GetBalance  - Returns current cheese count (for display).
//   CanAfford   - Checks if player has enough cheese for a purchase.
//   Spend       - Deducts cheese if affordable (returns false if not).
//   Add         - Adds cheese (e.g., from maze mini-game rewards).
// ---------------------------------------------------------------------------
int Credits_GetBalance() { return credits; }
bool Credits_CanAfford(int cost) { return credits >= cost; }

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
// Credits Loading from File and Saving to File
// ---------------------------------------------------------------------------
void Credits_LoadFile(const std::string& filename) {
    std::ifstream ifs(filename);
    if (ifs.is_open())
    {
        ifs >> credits;
        // if file empty/corrupt
        if (!ifs) credits = 0; // set back to 0 prevent loading of anything weird
        ifs.close();

        credits_init = credits; // set init credit amt to credits
    }
    else
    {
        // file doesn't exist credits remain at 0
    }
}
void Credits_SaveFile(const std::string& filename) {
    std::ofstream ofs(filename);
    if (ofs.is_open()) {
        ofs << credits; // append to file the new credits
    }
    ofs.close();
    credits_init = credits; // set init credit to amt credits
}
// Before any saving or loading to txtfile
// Normally should be different to credit value while inside a round of gameloop
int Credits_GetInitBalance() {
    return credits_init;
}