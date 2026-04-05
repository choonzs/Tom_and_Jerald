/*************************************************************************
@file    MainMenu.hpp
@Author  Loh Kai Xin kaixin.l@digipen.edu
@Co-authors Tan Choon Ming choonming.tan@digipen.edu,
			Ng Cher Kai Dan cherkaidan.ng@digipen.edu,
			Peng Rong Jun Jerald p.rongjunjerald@digipen.edu,
			Ong Jin Ting o.jinting@digipen.edu

@brief
     Declares the main menu game-state lifecycle functions and the
     gSkipSplash extern flag.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef MAINMENU_HPP
#define MAINMENU_HPP
void MainMenu_Load();

void MainMenu_Initialize();

void MainMenu_Update();

void MainMenu_Draw();

void MainMenu_Free();

void MainMenu_Unload();
#endif // !MAINMENU_H
