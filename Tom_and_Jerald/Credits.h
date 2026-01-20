#pragma once
#ifndef CREDITS_H
#define CREDITS_H

#include "pch.h"

const int k_credit_reward = 10;
const f32 k_credit_interval = 2.0f;

void Credits_ResetRound();
void Credits_Update(f32 delta_time, bool game_active);
void Credits_OnDamage();

int Credits_GetBalance();
bool Credits_CanAfford(int cost);
bool Credits_Spend(int cost);
void Credits_Add(int amount);

#endif // CREDITS_H
