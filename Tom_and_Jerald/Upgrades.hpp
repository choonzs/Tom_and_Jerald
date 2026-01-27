#pragma once
#ifndef UPGRADES_HPP
#define UPGRADES_HPP

#include "pch.hpp"

const int k_size_upgrade_max_level = 10;
const int k_health_upgrade_max_level = 5;
const f32 k_size_upgrade_step = 0.1f;
const f32 k_health_upgrade_step = 0.05f;

void Upgrades_Reset();

int Upgrades_GetSizeLevel();
int Upgrades_GetHealthLevel();

f32 Upgrades_GetSizeReduction();
f32 Upgrades_GetHealthIncrease();

bool Upgrades_CanUpgradeSize();
bool Upgrades_CanUpgradeHealth();

bool Upgrades_UpgradeSize();
bool Upgrades_UpgradeHealth();

#endif // UPGRADES_H