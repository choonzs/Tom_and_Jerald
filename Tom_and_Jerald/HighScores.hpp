/*************************************************************************
@file HighScores.hpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief
    Leaderboard:
       Stores up to 10 Score entries sorted in descending order. Entries are
       loaded from and saved to a whitespace-delimited text file (name score\n).
       AddScore() inserts a new entry, re-sorts, and trims to the top 10.

    Scene lifecycle:
       The HighScore scene loads the leaderboard file on Initialize, renders
       it each frame, and frees the Leaderboard object on Free.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#ifndef HIGHSCORES_HPP
#define HIGHSCORES_HPP

#include "pch.hpp"

// ===========================================================================
// Leaderboard
// ===========================================================================
class Leaderboard
{
public:
    struct Score
    {
        std::string name{};
        int         score{ 0 };
    };

    // Constructs the leaderboard by reading all name/score pairs from file.
    // The list is not sorted or capped on load — call AddScore() to insert
    // with sorting, or UpdateLeaderboard() to persist the current state.
    explicit Leaderboard(const std::string& filename);

    // Trims to the top 10 entries and writes them to file.
    void UpdateLeaderboard(const std::string& filename);

    // Inserts a score, re-sorts descending, and trims to 10 entries.
    void AddScore(const Score& newScore);

    // Returns the score of the last (lowest) entry, or 0 if empty.
    // Use this to check whether a new score qualifies for the board.
    int LowestScore() const;

    // Accessors
    const std::vector<Score>& HighScores() const { return highscores; }
    std::vector<Score>& HighScores() { return highscores; }

private:
    std::vector<Score> highscores;
};

// ---------------------------------------------------------------------------
// Scene lifecycle — registered with the Game State Manager
// ---------------------------------------------------------------------------
void HighScore_Load();
void HighScore_Initialize();
void HighScore_Update();
void HighScore_Draw();
void HighScore_Free();
void HighScore_Unload();

#endif // HIGHSCORES_HPP