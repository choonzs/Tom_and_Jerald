/*************************************************************************
@file Upgrades.hpp
@Author Tan Choon Ming choonming.tan@digipen.edu
@Co-authors Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@brief
    Interface for the upgrade system. Players spend cheese earned during
    gameplay to improve five stats, each capped at a maximum level.

     Stats and their per-level effects:
       Size        - Reduces player half_size by k_size_upgrade_step per level.
       Health      - Increases max health by k_health_upgrade_step (fractional).
       Fuel cap    - Multiplies max fuel by (1.0 + level * k_fuel_cap_upgrade_step).
       Fuel spawn  - Increases fuel pickup bonus by k_fuel_spawn_upgrade_step.
       Fuel restore- Increases fuel restore % (base 30% + k_fuel_restore_upgrade_step/level).

     Persistence:
       Levels are held in file-scope state. Use Upgrades_WriteToFile /
       Upgrades_ReadFromFile to persist them across sessions.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef UPGRADES_HPP
#define UPGRADES_HPP

#include "pch.hpp"

// ---------------------------------------------------------------------------
// Tuning constants
// ---------------------------------------------------------------------------

inline constexpr int k_size_upgrade_max_level = 10;
inline constexpr int k_health_upgrade_max_level = 5;
inline constexpr int k_fuel_upgrade_max_level = 10;

inline constexpr f32 k_size_upgrade_step = 0.1f;   // px reduction per level
inline constexpr f32 k_health_upgrade_step = 0.05f;  // fractional HP per level
inline constexpr f32 k_fuel_cap_upgrade_step = 0.10f;  // multiplier step per level
inline constexpr f32 k_fuel_spawn_upgrade_step = 0.05f;  // bonus fraction per level
inline constexpr f32 k_fuel_restore_upgrade_step = 0.05f;  // restore fraction per level

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

// Resets all upgrade levels to 0 (new save / test reset).
void Upgrades_Reset();

// ---------------------------------------------------------------------------
// Level getters — for Shop UI display
// ---------------------------------------------------------------------------

int Upgrades_GetSizeLevel();
int Upgrades_GetHealthLevel();
int Upgrades_GetFuelCapLevel();
int Upgrades_GetFuelSpawnLevel();
int Upgrades_GetFuelRestoreLevel();

// ---------------------------------------------------------------------------
// Computed stat getters — for gameplay application
// ---------------------------------------------------------------------------

// Total px to subtract from player half_size (level * k_size_upgrade_step).
f32 Upgrades_GetSizeReduction();

// Fractional health increase (level * k_health_upgrade_step).
f32 Upgrades_GetHealthIncrease();

// Fuel capacity multiplier (1.0 + level * k_fuel_cap_upgrade_step).
f32 Upgrades_GetMaxFuelMultiplier();

// Fuel spawn pickup bonus fraction (level * k_fuel_spawn_upgrade_step).
f32 Upgrades_GetFuelSpawnBonus();

// Fuel restore fraction (0.30 base + level * k_fuel_restore_upgrade_step).
f32 Upgrades_GetFuelRestorePercentage();

// ---------------------------------------------------------------------------
// Upgrade availability — for enabling/disabling Shop buttons
// ---------------------------------------------------------------------------

bool Upgrades_CanUpgradeSize();
bool Upgrades_CanUpgradeHealth();
bool Upgrades_CanUpgradeFuelCap();
bool Upgrades_CanUpgradeFuelSpawn();
bool Upgrades_CanUpgradeFuelRestore();

// ---------------------------------------------------------------------------
// Upgrade actions — called by Shop on confirmed purchase
// Each increments the level by 1 if not at max. Returns true on success.
// ---------------------------------------------------------------------------

bool Upgrades_UpgradeSize();
bool Upgrades_UpgradeHealth();
bool Upgrades_UpgradeFuelCap();
bool Upgrades_UpgradeFuelSpawn();
bool Upgrades_UpgradeFuelRestore();

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

// Reads all upgrade levels from a whitespace-delimited file.
// Returns false and prints to stderr if the file cannot be opened.
bool Upgrades_ReadFromFile(const std::string& filename);

// Writes all upgrade levels to a whitespace-delimited file.
// Returns false and prints to stderr if the file cannot be opened.
bool Upgrades_WriteToFile(const std::string& filename);

#endif // UPGRADES_HPP