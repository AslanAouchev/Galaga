#include "GalagaGameManager.h"
#include "GameObject.h"
#include <PlayerComponent.h>
#include <BackgroundScrollComponent.h>
#include "SceneManager.h"

GalagaGameManager::GalagaGameManager(dae::GameObject* pOwner) : dae::Component(pOwner)
{
}

void GalagaGameManager::OnNotify(const EventData& event)
{
    if (event.eventType == "PlayerHit")
    {
        HandlePlayerKilled(event);
        GetOwner()->TriggerEvent("PlayerHit");
    }
    else if (event.eventType == "ResumeKilled")
    {
        GetOwner()->TriggerEvent("ResumeKilled");
    }
    else if (event.eventType == "EnemyKilled")
    {
        HandleEnemyKilled(event);
        GetOwner()->TriggerEvent("EnemyKilled");
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
    }
}