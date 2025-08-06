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

    virtual void Update(float deltaTime) override;
    virtual void Render() const override;

    void SetPvPMode();
private:

    int m_PauseMenuSelection{ 0 };
    int m_Score{ 0 };
    int m_Lives{ 3 };
    int m_EnemiesKilled{ 0 };
    int m_TotalEnemies{ 0 };
    int m_ShotAmount{ 0 };
    int m_PlayerCount{ 0 };
    int m_DoOnce{};
    int m_CurrentLevel{ 1 };
    int m_PreviousObserverCount{};

    float m_BossDiveCooldown{ 0.0f };
    const float m_BossDiveCooldownDuration{ 3.0f };

    float m_GameOverTimer{ 8.f };
    const float m_GameOverDuration{ 2.0f };

    bool m_LevelStarted{ false };
    bool m_AllEnemiesSpawned{ false };
    bool m_pvpMode{};
    bool m_IsPaused{ false };
    bool m_EnteringName{ false };
    bool m_ShowingGameOver{ false };
    bool m_LivesInitialized{};

    float m_EnemySpawnTimer{  };
    float m_EnemySpawnDelay{ 2.0f };
    int m_EnemiesSpawned{};

    float m_EnemyAttackTimer{};
    float m_EnemyAttackInterval{3.0f};

    std::unique_ptr<dae::GameObject> m_LevelText;
    std::unique_ptr<dae::GameObject> m_ScoreDisplay;

    void HandlePlayerKilled(const EventData& event);
    void HandleEnemyKilled(const EventData& event);

    void CheckLevelComplete();

    void OnNotify(const EventData& event) override;

    void SetScore(const EventData& event);

    void PauseGame();
    void EndGameWin();
    void EndGameLose();

    void AddScore(int points);
    void SetLives(int lives) { m_Lives = lives; }
    void UpdatePauseMenuSelection();

    void CreateLifeDisplay();
    void UpdatePlayerCount();

    void UpdateLevelLogic(float deltaTime);
    void UpdateEnemyAttacks(float deltaTime);

    void StartLevel(int level);
    void SetupLevelParameters();
    void SpawnNextEnemy();
    void TriggerRandomEnemyAttack();
    void ClearEnemies();
    void ShowLevelStart();
    void SkipToNextLevel();

    void CreateScoreDisplay();
    void UpdateScoreDisplay();
};