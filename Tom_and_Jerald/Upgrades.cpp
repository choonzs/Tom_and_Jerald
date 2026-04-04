/*************************************************************************
@file Upgrades.cpp
@Author Tan Choon Ming choonming.tan@digipen.edu
@Co-authors Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@brief
    Implements the upgrade system. Players spend cheese to improve five stats,
    each capped at a maximum level defined in Upgrades.hpp.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "Upgrades.hpp"

// ---------------------------------------------------------------------------
// File-scope state
// ---------------------------------------------------------------------------

namespace
{
    int size_level = 0;
    int health_level = 0;
    int fuel_cap_level = 0;
    int fuel_spawn_level = 0;
    int fuel_restore_level = 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void Upgrades_Reset()
{
    size_level = 0;
    health_level = 0;
    fuel_cap_level = 0;
    fuel_spawn_level = 0;
    fuel_restore_level = 0;
}

// ---------------------------------------------------------------------------
// Level getters
// ---------------------------------------------------------------------------

int Upgrades_GetSizeLevel() { return size_level; }
int Upgrades_GetHealthLevel() { return health_level; }
int Upgrades_GetFuelCapLevel() { return fuel_cap_level; }
int Upgrades_GetFuelSpawnLevel() { return fuel_spawn_level; }
int Upgrades_GetFuelRestoreLevel() { return fuel_restore_level; }

// ---------------------------------------------------------------------------
// Computed stat getters
// ---------------------------------------------------------------------------

f32 Upgrades_GetSizeReduction()
{
    return size_level * k_size_upgrade_step;
}

f32 Upgrades_GetHealthIncrease()
{
    return health_level * k_health_upgrade_step;
}

f32 Upgrades_GetMaxFuelMultiplier()
{
    return 1.0f + (fuel_cap_level * k_fuel_cap_upgrade_step);
}

f32 Upgrades_GetFuelSpawnBonus()
{
    return fuel_spawn_level * k_fuel_spawn_upgrade_step;
}

f32 Upgrades_GetFuelRestorePercentage()
{
    return 0.30f + (fuel_restore_level * k_fuel_restore_upgrade_step);
}

// ---------------------------------------------------------------------------
// Upgrade availability
// ---------------------------------------------------------------------------

bool Upgrades_CanUpgradeSize() { return size_level < k_size_upgrade_max_level; }
bool Upgrades_CanUpgradeHealth() { return health_level < k_health_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelCap() { return fuel_cap_level < k_fuel_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelSpawn() { return fuel_spawn_level < k_fuel_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelRestore() { return fuel_restore_level < k_fuel_upgrade_max_level; }

// ---------------------------------------------------------------------------
// Upgrade actions
// ---------------------------------------------------------------------------

bool Upgrades_UpgradeSize()
{
    if (!Upgrades_CanUpgradeSize()) return false;
    ++size_level;
    return true;
}

bool Upgrades_UpgradeHealth()
{
    if (!Upgrades_CanUpgradeHealth()) return false;
    ++health_level;
    return true;
}

bool Upgrades_UpgradeFuelCap()
{
    if (!Upgrades_CanUpgradeFuelCap()) return false;
    ++fuel_cap_level;
    return true;
}

bool Upgrades_UpgradeFuelSpawn()
{
    if (!Upgrades_CanUpgradeFuelSpawn()) return false;
    ++fuel_spawn_level;
    return true;
}

bool Upgrades_UpgradeFuelRestore()
{
    if (!Upgrades_CanUpgradeFuelRestore()) return false;
    ++fuel_restore_level;
    return true;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

bool Upgrades_ReadFromFile(const std::string& filename)
{
    std::ifstream inFile(filename);
    if (!inFile)
    {
        std::cerr << "Upgrades: failed to open \"" << filename << "\" for reading.\n";
        return false;
    }

    inFile >> size_level >> health_level
        >> fuel_cap_level >> fuel_spawn_level >> fuel_restore_level;

    // If the read fails the file is empty or corrupt; reset to a safe state.
    if (!inFile)
        Upgrades_Reset();

    return true;
}

bool Upgrades_WriteToFile(const std::string& filename)
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        std::cerr << "Upgrades: failed to open \"" << filename << "\" for writing.\n";
        return false;
    }

    outFile << size_level << " "
        << health_level << " "
        << fuel_cap_level << " "
        << fuel_spawn_level << " "
        << fuel_restore_level;

    return true;
}