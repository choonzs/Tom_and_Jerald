#include "HighScores.hpp"

#include "pch.hpp"
#include "Utils.hpp"
#include "Audio.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"

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
const int& Leaderboard::LowestScore() {
	static const int default_score{ 0 };
	if (highscores.empty()) {
		return default_score;
	}
	else {
		return highscores.back().score;

	}
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
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
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
	const std::vector<Leaderboard::Score> DeepCopyHighScores(currentboard->HighScores());
	for (Leaderboard::Score entry : DeepCopyHighScores) {
		y_pos -= 0.1f;
		// ===== FORMATTED OUTPUT FOR LEADERBOARD ENTRY ============================
		std::stringstream ss;
		ss << entry.name;

		for (size_t i{ entry.name.size() }; i < 20; ++i) {
			ss << " ";
		}
		ss << entry.score;
		std::string formatted_entry = ss.str();
		// =========================================================================
		drawCenteredText(font_id, formatted_entry.c_str(), y_pos, 0.8f, 0.0f, 0.0f);
	}
}
void HighScore_Free() {

}
void HighScore_Unload() {
	AEGfxDestroyFont(font_id);
	// dealloc memory for current board
	delete currentboard;
	currentboard = nullptr;
}

