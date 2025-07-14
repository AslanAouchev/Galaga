#include "GalagaGameManager.h"
#include "GameObject.h"
#include <PlayerComponent.h>
#include <BackgroundScrollComponent.h>

GalagaGameManager::GalagaGameManager(dae::GameObject* pOwner) : dae::Component(pOwner)
{
}

void GalagaGameManager::OnNotify(const EventData& event)
{
    if (!m_IsGameRunning && event.eventType != "PauseButton")
    {
        return;
    }

    if (event.eventType == "PlayerHit")
    {
        HandlePlayerKilled(event);
		GetOwner()->TriggerEvent("PlayerHit");
    }
    else if (event.eventType == "ResumeKilled")
    {
        HandleEnemyKilled(event);
        GetOwner()->TriggerEvent("ResumeKilled");
    }
    else if (event.eventType == "EnemyKilled")
    {
        HandleEnemyKilled(event);
        GetOwner()->TriggerEvent("EnemyKilled");
    }
    else if (event.eventType == "PauseButton")
    {
		if (!m_IsPaused && m_IsGameRunning)
		{
			PauseGame();
            GetOwner()->TriggerEvent("Pause");
		}
        else
        {
			ResumeGame();
            GetOwner()->TriggerEvent("Resume");
        }
    }
}

void GalagaGameManager::StartGame()
{
    m_IsGameRunning = true;
    m_IsGameOver = false;
    m_IsPaused = false;
    UpdateUI();
}

void GalagaGameManager::PauseGame()
{
    if (m_IsGameRunning && !m_IsPaused)
    {
        m_IsPaused = true;
        m_IsGameRunning = false;
        UpdateUI();
    }
}

void GalagaGameManager::ResumeGame()
{
    if (m_IsPaused)
    {
        m_IsPaused = false;
        m_IsGameRunning = true;
        UpdateUI();
    }
}

void GalagaGameManager::EndGame()
{
    m_IsGameRunning = false;
    m_IsGameOver = true;
    UpdateUI();
}

void GalagaGameManager::ResetGame()
{
    m_Score = 0;
    m_Lives = 3;
    m_EnemiesKilled = 0;
    m_IsGameOver = false;
    m_IsPaused = false;
    m_IsGameRunning = true;
    UpdateUI();
}

void GalagaGameManager::AddScore(int points)
{
    m_Score += points;
    UpdateUI();
}

void GalagaGameManager::HandlePlayerKilled(const EventData& )
{
    --m_Lives;
    if (m_Lives <= 0)
    {
        EndGame();
    }

    UpdateUI();
}

void GalagaGameManager::HandleEnemyKilled(const EventData& )
{
    ++m_EnemiesKilled;
    //const int points{ event.value > 0 ? event.value : 100 };
    //AddScore(points);

    CheckLevelComplete();
}

void GalagaGameManager::UpdateUI()
{
    if (m_IsPaused)
    {
        ;
    }
    if (m_IsGameOver)
    {
        ;
    }
}

void GalagaGameManager::CheckLevelComplete()
{
    if (m_EnemiesKilled >= m_TotalEnemies)
    {
        AddScore(1000);
    }
}