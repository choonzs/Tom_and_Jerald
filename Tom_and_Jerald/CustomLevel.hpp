/*************************************************************************
@file    CustomLevel.hpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  NIL
@brief
     Declares the Custom Level game-state lifecycle functions.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#pragma once
#ifndef CUSTOM_LEVEL_HPP
#define CUSTOM_LEVEL_HPP

void CustomLevel_Load();
void CustomLevel_Initialize();
void CustomLevel_Update();
void CustomLevel_Draw();
void CustomLevel_Free();
void CustomLevel_Unload();

#endif // !CUSTOM_LEVEL_HPP