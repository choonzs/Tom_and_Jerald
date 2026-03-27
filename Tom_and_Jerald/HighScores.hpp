#include "pch.hpp"

#ifndef HIGHSCORES_HPP
#define HIGHSCORES_HPP
// CLASS TO STORE LEADERBOARD

class Leaderboard {
	public:
		struct Score {
			std::string name{};
			int score{ 0 };
		};

	private:	
		std::vector<Score> highscores;

	public:
		//Construct using textfile
		Leaderboard(std::string const& filename);

		// Update textfile
		void UpdateLeaderboard(std::string const& filename);
		void AddScore(Score const& newScore);
		const int& LowestScore();
		
		// Accessor & Mutator of vector
		std::vector<Score>const& HighScores() const { return highscores; }
		std::vector<Score>& HighScores() { return highscores; }

};

// SCENE MANAGEMENT
void HighScore_Load();
void HighScore_Initialize();
void HighScore_Update();
void HighScore_Draw();
void HighScore_Free();
void HighScore_Unload();

#endif