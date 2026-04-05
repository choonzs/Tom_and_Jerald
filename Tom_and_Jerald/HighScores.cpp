/*************************************************************************
@file HighScores.cpp
@Author Ng Cher Kai Dan cherkaidan.ng@digipen.edu
@Co-authors NIL
@brief  Implements the Leaderboard class and the HighScore scene.

Copyright � 2026 DigiPen, All rights reserved.
*************************************************************************/

#include "pch.hpp"
#include "HighScores.hpp"
#include "Utils.hpp"
#include "Audio.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "ImgFontInit.hpp"

// ===========================================================================
// Leaderboard
// ===========================================================================

// ---------------------------------------------------------------------------
// Leaderboard::Leaderboard
// ---------------------------------------------------------------------------
// Reads all name/score pairs from the file into highscores. The list is
// stored in file order; no sorting or capping is applied on load.
// ---------------------------------------------------------------------------
Leaderboard::Leaderboard(const std::string& filename)
{
    std::ifstream ifs(filename);
    Score temp;
    while (ifs >> temp.name >> temp.score)
        highscores.push_back(temp);
}

// ---------------------------------------------------------------------------
// Leaderboard::UpdateLeaderboard
// ---------------------------------------------------------------------------
// Trims the list to 10 entries (removing lowest scores) then overwrites
// the file with the current contents.
// ---------------------------------------------------------------------------
void Leaderboard::UpdateLeaderboard(const std::string& filename)
{
    while (highscores.size() > 10)
        highscores.pop_back();

    std::ofstream ofs(filename);
    for (const Score& entry : highscores)
        ofs << entry.name << " " << entry.score << '\n';
}

// ---------------------------------------------------------------------------
// Leaderboard::AddScore
// ---------------------------------------------------------------------------
// Appends the new score, sorts the list descending by score, then trims to
// the top 10. After this call the vector is always sorted and capped.
// ---------------------------------------------------------------------------
void Leaderboard::AddScore(const Score& score)
{
    highscores.push_back(score);

    std::sort(highscores.begin(), highscores.end(),
        [](const Score& a, const Score& b) { return a.score > b.score; });

    while (highscores.size() > 10)
        highscores.pop_back();
}

// ---------------------------------------------------------------------------
// Leaderboard::LowestScore
// ---------------------------------------------------------------------------
// Returns the score of the last entry (lowest on a sorted board), or 0 if
// the board is empty. Use this to quickly test whether a new score qualifies.
// ---------------------------------------------------------------------------
int Leaderboard::LowestScore() const
{
    return highscores.empty() ? 0 : highscores.back().score;
}

// ===========================================================================
// HighScore scene � file-scope state
// ===========================================================================

namespace
{
    s8          font_id = 0;
    Leaderboard* currentboard = nullptr;
}

// ===========================================================================
// Scene lifecycle
// ===========================================================================

void HighScore_Load()
{
    ASSETS::Init_Font();
    font_id = ASSETS::Font();
}

void HighScore_Initialize()
{
    currentboard = new Leaderboard("Assets/data/HighScores.txt");
}

void HighScore_Update()
{
    if (AEInputCheckTriggered(AEVK_ESCAPE) || !AESysDoesWindowExist()) {
        gSkipSplash = true;
        next = GAME_STATE_MENU;
    }
}

// ---------------------------------------------------------------------------
// HighScore_Draw
// ---------------------------------------------------------------------------
// Renders the leaderboard title then each entry as a name on the left column
// and a right-aligned score on the right column. Score width is measured via
// AEGfxGetPrintSize so it anchors correctly regardless of digit count.
// ---------------------------------------------------------------------------
void HighScore_Draw()
{
    f32 y_pos = 0.5f;
    drawCenteredText(font_id, "LEADERBOARD", y_pos, 1.1f, 0.0f, 0.0f, 1.0f, 0.0f);

    constexpr f32 LEFT_X = -0.1f;
    constexpr f32 RIGHT_X = 0.1f;

    for (const Leaderboard::Score& entry : currentboard->HighScores())
    {
        y_pos -= 0.1f;
        drawText(font_id, entry.name.c_str(), 0.8f, LEFT_X, y_pos);

        const std::string score_str = std::to_string(entry.score);
        f32 score_width = 0.0f, score_height = 0.0f;
        AEGfxGetPrintSize(font_id, score_str.c_str(), 0.8f, &score_width, &score_height);
        drawText(font_id, score_str.c_str(), 0.8f, RIGHT_X - score_width, y_pos);
    }
}

void HighScore_Free()
{
    delete currentboard;
    currentboard = nullptr;
}

void HighScore_Unload()
{
    ASSETS::Unload_Font();
}
