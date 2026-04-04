#include "HighScores.hpp"

#include "pch.hpp"
#include "Utils.hpp"
#include "Audio.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "ImgFontInit.hpp"

Leaderboard::Leaderboard(std::string const& filename) {
	std::ifstream ifs(filename);
	Score temp;
	while (ifs >> temp.name >> temp.score) {
		highscores.push_back(temp);
	}
	ifs.close();
}

void Leaderboard::UpdateLeaderboard(std::string const& filename) {
	// Only want to store top 10
	while (highscores.size() > 10) {
		highscores.pop_back();
	}

	// Write to output
	std::ofstream ofs(filename);
	for (Score entry : highscores) {
		ofs << entry.name << " " << entry.score << '\n';
	}
	ofs.close();
}
const int Leaderboard::LowestScore() {
	static const int default_score{ 0 };
	if (highscores.empty()) return 0;
	return highscores.back().score;
}

void Leaderboard::AddScore(Score const& score) {
	
	highscores.push_back(score);

	// Sort it in the right order descending
	std::sort(highscores.begin(), highscores.end(),
		[](const Score& a, const Score& b) {
			return a.score > b.score; });

	// Only want to store top 10
	while (highscores.size() > 10) {
		highscores.pop_back();
	}
}
// ===================================================================================
namespace {
	s8 font_id;
	Leaderboard* currentboard = nullptr;
}
// ===================================================================================
// SCENE MANAGEMENT
void HighScore_Load() {
	ASSETS::Init_Font();
	font_id = ASSETS::Font();
}
void HighScore_Initialize() {
	currentboard = new Leaderboard("Assets/data/HighScores.txt");
}
void HighScore_Update() {
	if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
	{
		//PlayClick();
		// Return to previous page
		next = GAME_STATE_MENU;
	}
}

void HighScore_Draw() {
	f32 y_pos{ 0.5f };
	drawCenteredText(font_id, "LEADERBOARD", y_pos, 1.1f, 0.0f, 0.0f, 1.0f, 0.0f);
	const std::vector<Leaderboard::Score>& scores = currentboard->HighScores();
	for (Leaderboard::Score entry : scores) {
		y_pos -= 0.1f;
		// ===== FORMATTED OUTPUT FOR LEADERBOARD ENTRY ============================
		//std::stringstream ss;
		//ss << std::left << std::setw(20) << entry.name;

		//ss << std::right << std::setw(5) << entry.score;
		//std::string formatted_entry = ss.str();
		//// =========================================================================
		//drawCenteredText(font_id, formatted_entry.c_str(), y_pos, 0.8f, 0.0f, 0.0f);
		const f32 LEFT_X = -0.1f;
		drawText(font_id, entry.name.c_str(), 0.8f, LEFT_X, y_pos);

		// Draw score right-anchored at a fixed right column
		const f32 RIGHT_X = 0.1f;
		std::string score_str = std::to_string(entry.score);
		f32 score_width = 0.0f, score_height = 0.0f;
		AEGfxGetPrintSize(font_id, score_str.c_str(), 0.8f, &score_width, &score_height);
		drawText(font_id, score_str.c_str(), 0.8f, RIGHT_X - score_width, y_pos);
	}
}
void HighScore_Free() {
	delete currentboard;
	currentboard = nullptr;
}
void HighScore_Unload() {
	ASSETS::Unload_Font();
	// dealloc memory for current board
}

