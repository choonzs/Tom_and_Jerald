#include "pch.hpp"
#include "Upgrades.hpp"

namespace {
	int size_level = 0;
	int health_level = 0;
	int fuel_cap_level = 0;
	int fuel_spawn_level = 0;
	int fuel_restore_level = 0;
}

void Upgrades_Reset()
{
	size_level = 0;
	health_level = 0;
	fuel_cap_level = 0;
	fuel_spawn_level = 0;
	fuel_restore_level = 0;
}

int Upgrades_GetSizeLevel() { return size_level; }
int Upgrades_GetHealthLevel() { return health_level; }
int Upgrades_GetFuelCapLevel() { return fuel_cap_level; }
int Upgrades_GetFuelSpawnLevel() { return fuel_spawn_level; }
int Upgrades_GetFuelRestoreLevel() { return fuel_restore_level; }

f32 Upgrades_GetSizeReduction() { return size_level * k_size_upgrade_step; }
f32 Upgrades_GetHealthIncrease() { return health_level * k_health_upgrade_step; }
f32 Upgrades_GetMaxFuelMultiplier() { return 1.0f + (fuel_cap_level * k_fuel_cap_upgrade_step); }
f32 Upgrades_GetFuelSpawnBonus() { return fuel_spawn_level * k_fuel_spawn_upgrade_step; }
f32 Upgrades_GetFuelRestorePercentage() { return 0.30f + (fuel_restore_level * k_fuel_restore_upgrade_step); }

bool Upgrades_CanUpgradeSize() { return size_level < k_size_upgrade_max_level; }
bool Upgrades_CanUpgradeHealth() { return health_level < k_health_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelCap() { return fuel_cap_level < k_fuel_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelSpawn() { return fuel_spawn_level < k_fuel_upgrade_max_level; }
bool Upgrades_CanUpgradeFuelRestore() { return fuel_restore_level < k_fuel_upgrade_max_level; }

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