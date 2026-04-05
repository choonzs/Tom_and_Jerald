/*************************************************************************
@file    Playing.hpp
@Author  Loh Kai Xin kaixin.l@digipen.edu
@Co-authors  Jerald, Ong Jinting
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
