#include "HighScoreManager.h"
#include "json.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

std::string HighScoreManager::GetHighScoreFilePath() const
{
    return "../Data/highscores.json";
}

std::vector<HighScore> HighScoreManager::LoadHighScores()
{
    std::vector<HighScore> scores;
    std::ifstream file(GetHighScoreFilePath());

    if (!file.is_open())
    {
        std::cout << "High score file not found" << std::endl;
        return scores;
    }

    try
    {
        json jsonData;
        file >> jsonData;
        file.close();

        if (jsonData.contains("highscores") && jsonData["highscores"].is_array())
        {
            for (const auto& scoreEntry : jsonData["highscores"])
            {
                if (scoreEntry.contains("name") && scoreEntry.contains("score"))
                {
                    std::string name = scoreEntry["name"];
                    int score = scoreEntry["score"];

                    if (!name.empty() && score > 0)
                    {
                        scores.emplace_back(name, score);
                    }
                }
            }
        }

        std::sort(scores.begin(), scores.end(),
            [](const HighScore& a, const HighScore& b) {
                return a.score > b.score;
            });

        if (scores.size() > MAX_HIGH_SCORES)
            scores.resize(MAX_HIGH_SCORES);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error loading high scores " << e.what() << std::endl;
        scores.clear();
    }

    return scores;
}

void HighScoreManager::SaveHighScores(const std::vector<HighScore>& scores)
{
    try
    {
        json jsonData;
        jsonData["highscores"] = json::array();

        for (const auto& score : scores)
        {
            json scoreEntry;
            scoreEntry["name"] = score.playerName;
            scoreEntry["score"] = score.score;
            jsonData["highscores"].push_back(scoreEntry);
        }

        std::ofstream file(GetHighScoreFilePath());
        if (file.is_open())
        {
            file << jsonData.dump(2);
            file.close();
            std::cout << "High scores saved successfully" << std::endl;
        }
        else
        {
            std::cout << "Could not open high scores file" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Error saving high scores " << e.what() << std::endl;
    }
}

bool HighScoreManager::IsHighScore(int score)
{
    auto scores{ LoadHighScores() };

    if (scores.size() < MAX_HIGH_SCORES)
    {
        return true;
    }

    return score > scores.back().score;
}

int HighScoreManager::GetHighScorePosition(int score)
{
    auto scores{ LoadHighScores() };

    for (int i{}; i < static_cast<int>(scores.size()); ++i)
    {
        if (score > scores[i].score)
        {
            return i + 1;
        }
    }

    if (scores.size() < MAX_HIGH_SCORES)
    {
        return static_cast<int>(scores.size()) + 1;
    }

    return -1;
}

void HighScoreManager::AddHighScore(const std::string& name, int score)
{
    auto scores{ LoadHighScores() };

    std::string playerName{ name };
    std::transform(playerName.begin(), playerName.end(), playerName.begin(),
        [](char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); });

    if (playerName.length() > 3)
    {
        playerName = playerName.substr(0, 3);
    }

    while (playerName.length() < 3)
    {
        playerName += " ";
    }

    HighScore newScore(playerName, score);

    auto insertPos{ scores.begin() };

    for (auto it{ scores.begin() }; it != scores.end(); ++it)
    {
        if (score > it->score)
        {
            insertPos = it;
            break;
        }
        insertPos = it + 1;
    }

    scores.insert(insertPos, newScore);

    if (scores.size() > MAX_HIGH_SCORES)
    {
        scores.resize(MAX_HIGH_SCORES);
    }

    SaveHighScores(scores);
}

std::vector<HighScore> HighScoreManager::GetTopScores()
{
    return LoadHighScores();
}