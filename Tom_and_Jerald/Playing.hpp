#ifndef PLAYING_HPP
#define PLAYING_HPP
#include "pch.hpp"



struct Player
{
	AEVec2 position;
	AEVec2 half_size;
	int health;
};
// Temp placement of stuff until asiohjfioheiohodeiqj
const int k_max_health = 10;
const int k_obstacle_count = 10;
const f32 k_stage_duration = 30.0f;
const f32 k_player_speed = 500.0f;
const f32 k_damage_cooldown = 0.4f;
const f32 k_player_base_half_size = 25.0f;


void Playing_Load();

void Playing_Initialize();

void Playing_Update();

void Playing_Draw();

void Playing_Free();

void Playing_Unload();


#endif // !PLAYING_HPP
