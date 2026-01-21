#include "pch.h"
#include "Credits.h"

namespace {
	int credits = 1000;
	f32 no_damage_timer = 0.0f;
}

void Credits_ResetRound()
{
	no_damage_timer = 0.0f;
}

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

int Credits_GetBalance()
{
	return credits;
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