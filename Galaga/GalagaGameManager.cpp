#include "GalagaGameManager.h"
#include "GameObject.h"
#include <PlayerComponent.h>
#include <BackgroundScrollComponent.h>
#include "SceneManager.h"
#include "HighScoreManager.h"
GalagaGameManager::GalagaGameManager(dae::GameObject* pOwner) : dae::Component(pOwner)
{
}

void GalagaGameManager::OnNotify(const EventData& event)
{
    if (event.eventType == "PlayerHit")
    {
        HandlePlayerKilled(event);
        GetOwner()->TriggerEvent("ManagerPlayerHit");
    }
    else if (event.eventType == "PlayerKilled")
    {
        GetOwner()->TriggerEvent("ResumeKilled");
    }
    else if (event.eventType == "EnemyKilled")
    {
        HandleEnemyKilled(event);
    }
    else if (event.eventType == "Pause")
    {
        if (!m_IsPaused)
        {
            PauseGame();
            GetOwner()->TriggerEvent("PauseUI");
        }
        else if (m_IsPaused)
        {
            GetOwner()->TriggerEvent("Resume");
        }
    }
    else if (event.eventType == "MenuUp")
    {
        if (m_IsPaused)
        {
            m_PauseMenuSelection = (m_PauseMenuSelection - 1 + 2) % 2;
            UpdatePauseMenuSelection();
        }
    }
    else if (event.eventType == "MenuDown")
    {
        if (m_IsPaused)
        {
            m_PauseMenuSelection = (m_PauseMenuSelection + 1) % 2;
            UpdatePauseMenuSelection();
        }
    }
    else if (event.eventType == "MenuConfirm")
    {
        if (m_IsPaused)
        {
            if (m_PauseMenuSelection == 0)
            {
                m_IsPaused = false;
                GetOwner()->TriggerEvent("Resume");
            }
            else
            {
                auto& sceneManager = dae::SceneManager::GetInstance();

                extern void loadMainMenu();
                loadMainMenu();
                sceneManager.SetActiveScene("MainMenu");
            }
        }
    }
}

void GalagaGameManager::UpdatePauseMenuSelection()
{
    GetOwner()->TriggerEvent("DeselectAll");
    GetOwner()->TriggerEvent("Select" + std::to_string(m_PauseMenuSelection));
}

void GalagaGameManager::SetScore(const EventData& event)
{
    AddScore(event.gameObject->GetComponent<dae::PlayerComponent>()->GetScore());
}

void GalagaGameManager::PauseGame()
{
    m_IsPaused = true;
    m_PauseMenuSelection = 0;
    UpdatePauseMenuSelection();
}

void GalagaGameManager::EndGame()
{
    m_IsGameOver = true;

    auto& highScoreManager = HighScoreManager::GetInstance();

    if (highScoreManager.IsHighScore(m_Score))
    {
        int position = highScoreManager.GetHighScorePosition(m_Score);
        std::cout << "NEW HIGH SCORE! Position " << position << std::endl;

        std::string playerName = "AAA";
        OnNameInputComplete(playerName);
    }
    else
    {
        extern void loadHighScores();
        loadHighScores();
    }
}

void GalagaGameManager::OnNameInputComplete(const std::string& name)
{
    auto& highScoreManager = HighScoreManager::GetInstance();
    highScoreManager.AddHighScore(name, m_Score);

    std::cout << "High score saved " << name << " - " << m_Score << std::endl;

    extern void loadHighScores();
    loadHighScores();
}

void GalagaGameManager::ResetGame()
{
    m_Score = 0;
    m_Lives = 3;
    m_EnemiesKilled = 0;
    m_IsGameOver = false;
    m_IsPaused = false;
}

void GalagaGameManager::AddScore(int points)
{
    m_Score += points;
}

void GalagaGameManager::HandlePlayerKilled(const EventData& )
{
    --m_Lives;
    if (m_Lives <= 0)
    {
        EndGame();
    }
}

void GalagaGameManager::HandleEnemyKilled(const EventData& )
{
    ++m_EnemiesKilled;
    //const int points{ event.value > 0 ? event.value : 100 };
    //AddScore(points);

    CheckLevelComplete();
}

void GalagaGameManager::CheckLevelComplete()
{
    if (m_EnemiesKilled >= m_TotalEnemies)
    {
        
        AddScore(1000);

        std::cout << "Enemy killed, total enemies killed: " << m_EnemiesKilled << std::endl;
        std::cout << "Total enemies: " << m_Score << std::endl;
        EndGame();
    }
}