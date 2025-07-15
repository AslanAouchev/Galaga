#pragma once
#include "Observer.h"
#include <vector>
#include "Component.h"

namespace dae
{
    class GameObject;
}

class GalagaGameManager : public Observer, public dae::Component
{
public:
    GalagaGameManager(dae::GameObject* pOwner);
    ~GalagaGameManager() = default;

    GalagaGameManager(const GalagaGameManager& other) = delete;
    GalagaGameManager(GalagaGameManager&& other) = delete;
    GalagaGameManager& operator=(const GalagaGameManager& other) = delete;
    GalagaGameManager& operator=(GalagaGameManager&& other) = delete;

	virtual void Update(float ) override {}
	virtual void Render() const override {}

private:
    int m_PauseMenuSelection{ 0 };

    int m_Score{ 0 };
    int m_Lives{ 3 };
    bool m_IsGameOver{ false };
    bool m_IsPaused{ false };
    int m_EnemiesKilled{ 0 };
    int m_TotalEnemies{ 0 };

    void HandlePlayerKilled(const EventData& event);
    void HandleEnemyKilled(const EventData& event);

    void CheckLevelComplete();

    void OnNotify(const EventData& event) override;

    void SetScore(const EventData& event);

    void PauseGame();
    void EndGame();
    void ResetGame();

    void AddScore(int points);
    void SetLives(int lives) { m_Lives = lives; }
    void UpdatePauseMenuSelection();
};