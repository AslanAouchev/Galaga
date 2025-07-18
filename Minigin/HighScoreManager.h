#pragma once
#include "Singleton.h"
#include <vector>
#include <string>

struct HighScore
{
    std::string playerName;
    int score;

    HighScore() : playerName(""), score(0) {}
    HighScore(const std::string& name, int playerScore)
        : playerName(name), score(playerScore) {
    }
};

class HighScoreManager : public dae::Singleton<HighScoreManager>
{
public:
    std::vector<HighScore> LoadHighScores();
    void SaveHighScores(const std::vector<HighScore>& scores);
    bool IsHighScore(int score);
    int GetHighScorePosition(int score);
    void AddHighScore(const std::string& name, int score);
    std::vector<HighScore> GetTopScores();

private:
    friend class dae::Singleton<HighScoreManager>;

    HighScoreManager() = default;

    std::string GetHighScoreFilePath() const;

    static const int MAX_HIGH_SCORES = 5;
};