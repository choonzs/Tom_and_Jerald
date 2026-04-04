/*************************************************************************
@file GameStateList.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Defines the GameState enum used by the game state manager to identify
    every screen and mode in the application.

    Special values:
    GAME_STATE_RESTART - Signals the state manager to reload the current
                        state from scratch without transitioning elsewhere.
    GAME_STATE_QUIT    - Signals the application loop to exit cleanly.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef GAME_STATE_LIST_HPP
#define GAME_STATE_LIST_HPP

enum GameState
{
	GAME_STATE_MENU = 0,
	GAME_STATE_SHOP,
	GAME_STATE_TUTORIAL,
	GAME_STATE_PLAYING,
	GAME_STATE_MAZE,
	GAME_STATE_LEVEL_EDITOR,
	GAME_STATE_CUSTOM_PLAY,
	GAME_STATE_SETTINGS,
	GAME_STATE_GAME_OVER,
	GAME_STATE_VICTORY,
	GAME_STATE_HIGHSCORE,
	GAME_STATE_CREDITS,
	GAME_STATE_RESTART,
	GAME_STATE_QUIT
};

#endif // GAME_STATE_LIST_HPP