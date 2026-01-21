#include "pch.h"
#include "Upgrades.hpp"

namespace {
	int size_level = 0;
	int health_level = 0;
}

void Upgrades_Reset()
{
	size_level = 0;
	health_level = 0;
}

int Upgrades_GetSizeLevel()
{
	return size_level;
}

int Upgrades_GetHealthLevel()
{
	return health_level;
}

f32 Upgrades_GetSizeReduction()
{
	return size_level * k_size_upgrade_step;
}

f32 Upgrades_GetHealthIncrease()
{
	return health_level * k_health_upgrade_step;
}

bool Upgrades_CanUpgradeSize()
{
	return size_level < k_size_upgrade_max_level;
}

bool Upgrades_CanUpgradeHealth()
{
	return health_level < k_health_upgrade_max_level;
}

bool Upgrades_UpgradeSize()
{
	if (!Upgrades_CanUpgradeSize())
		return false;

	++size_level;
	return true;
}

bool Upgrades_UpgradeHealth()
{
	if (!Upgrades_CanUpgradeHealth())
		return false;

	++health_level;
	return true;
}