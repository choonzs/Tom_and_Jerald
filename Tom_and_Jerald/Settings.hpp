#pragma once
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "AEEngine.h"
#include "Audio.hpp"

extern f32 bgmVolume;    // current background music volume
extern f32 seVolume;     // current sound effect volume
extern f32 sliderX_bgm; // current bgm slider handle position
extern f32 sliderX_se;  // current se slider handle position

void Setting_Load();
void Settings_Initialize();
void Settings_Update();
void Settings_Draw();
void Settings_Free();
void Settings_Unload();

#endif