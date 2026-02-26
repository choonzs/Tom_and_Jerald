#pragma once
#ifndef UPGRADES_HPP
#define UPGRADES_HPP

#include "pch.hpp"

const int k_size_upgrade_max_level = 10;
const int k_health_upgrade_max_level = 5;
const int k_fuel_upgrade_max_level = 10;

const f32 k_size_upgrade_step = 0.1f;
const f32 k_health_upgrade_step = 0.05f;
const f32 k_fuel_cap_upgrade_step = 0.10f;
const f32 k_fuel_spawn_upgrade_step = 0.05f;
const f32 k_fuel_restore_upgrade_step = 0.05f;

void Upgrades_Reset();

int Upgrades_GetSizeLevel();
int Upgrades_GetHealthLevel();
int Upgrades_GetFuelCapLevel();
int Upgrades_GetFuelSpawnLevel();
int Upgrades_GetFuelRestoreLevel();

f32 Upgrades_GetSizeReduction();
f32 Upgrades_GetHealthIncrease();
f32 Upgrades_GetMaxFuelMultiplier();
f32 Upgrades_GetFuelSpawnBonus();
f32 Upgrades_GetFuelRestorePercentage();

bool Upgrades_CanUpgradeSize();
bool Upgrades_CanUpgradeHealth();
bool Upgrades_CanUpgradeFuelCap();
bool Upgrades_CanUpgradeFuelSpawn();
bool Upgrades_CanUpgradeFuelRestore();

bool Upgrades_UpgradeSize();
bool Upgrades_UpgradeHealth();
bool Upgrades_UpgradeFuelCap();
bool Upgrades_UpgradeFuelSpawn();
bool Upgrades_UpgradeFuelRestore();

#endif // UPGRADES_HPP