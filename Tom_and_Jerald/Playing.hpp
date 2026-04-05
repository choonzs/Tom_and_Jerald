/*************************************************************************
@file    Playing.hpp
@Author  Tan Choon Ming choonming.tan@digipen.edu
@Co-authors  Ng Cher Kai Dan cherkaidan.ng@digipen.edu
Loh Kai Xin kaixin.l@digipen.edu,
Peng Jun Rong Jerald p.rongjunjerald@digipen.edu,
Ong Jin Ting o.jinting@digipen.edu
@brief
     Declares the Playing game-state lifecycle functions, the saved
     state snapshot struct, and shared gameplay helper types.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#pragma once
#ifndef PLAYING_HPP
#define PLAYING_HPP
#include "pch.hpp"

void Playing_Load();

void Playing_Initialize();

void Playing_Update();

void Playing_Draw();

void Playing_Free();

void Playing_Unload();


#endif // !PLAYING_HPP
