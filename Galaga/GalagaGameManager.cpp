#include "GalagaGameManager.h"
#include "GameObject.h"
#include <PlayerComponent.h>
#include <BackgroundScrollComponent.h>
#include "SceneManager.h"
#include "HighScoreManager.h"
#include "Scene.h"
#include "NameInputComponent.h"

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
        if(!m_EnteringName)
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
    }
    else if (event.eventType == "MenuUp")
    {
        if (m_IsPaused && !m_EnteringName)
        {
            m_PauseMenuSelection = (m_PauseMenuSelection - 1 + 2) % 2;
            UpdatePauseMenuSelection();
        }
        else if (m_EnteringName)
        {
            GetOwner()->TriggerEvent("NameUp");
        }
    }
    else if (event.eventType == "MenuDown")
    {
        if (m_IsPaused && !m_EnteringName)
        {
            m_PauseMenuSelection = (m_PauseMenuSelection + 1) % 2;
            UpdatePauseMenuSelection();
        }
        else if (m_EnteringName)
        {
			GetOwner()->TriggerEvent("NameDown");
        }
    }
    else if (event.eventType == "MenuConfirm")
    {
        if (m_IsPaused && !m_EnteringName)
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
        else if (m_EnteringName)
        {
            m_EnteringName = false;
			GetOwner()->TriggerEvent("NameInputComplete");
        }
    }
    else if (event.eventType == "Shot")
    {
        ++m_ShotAmount;
    }
    else if (event.eventType == "NameLeft" && m_EnteringName)
    {
        GetOwner()->TriggerEvent("NameLeftComponent");
    }
    else if (event.eventType == "NameRight" && m_EnteringName)
    {
        GetOwner()->TriggerEvent("NameRightComponent");
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
    GetOwner()->TriggerEvent("GameOver");

    float accuracy{};
    if (m_ShotAmount > 0)
    {
        accuracy = (static_cast<float>(m_EnemiesKilled) / static_cast<float>(m_ShotAmount)) * 100.0f;
    }

    auto& highScoreManager = HighScoreManager::GetInstance();

    if (highScoreManager.IsHighScore(m_Score))
    {
        GetOwner()->AddComponent<NameInputComponent>(GetOwner(), m_Score, m_ShotAmount, m_EnemiesKilled, accuracy);

        GetOwner()->AddObserver(GetOwner()->GetComponent<NameInputComponent>());

		GetOwner()->TriggerEvent("EndGame");

        m_EnteringName = true;
    }
    else
    {
        extern void loadHighScores();
        loadHighScores();
    }
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

void GalagaGameManager::HandleEnemyKilled(const EventData& event)
{
    ++m_EnemiesKilled;
    const int points{ event.intValue };
    AddScore(points);
    std::cout << "lol: " << points << std::endl;

    CheckLevelComplete();
}

void GalagaGameManager::CheckLevelComplete()
{
    if (m_EnemiesKilled >= m_TotalEnemies)
    {
        AddScore(1000);

        EndGame();
    }
}

void GalagaGameManager::CountEnemiesInScene()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto& allGameObjects = scene.GetAllGameObjects();

    int enemyCount{};

    for (const auto& gameObject : allGameObjects)
    {
        if (gameObject)
        {
            auto playerComponent = gameObject->GetComponent<dae::PlayerComponent>();
            if (playerComponent)
            {
                auto tag = playerComponent->GetTag();
                if (tag == dae::GameObjectTag::Bee ||
                    tag == dae::GameObjectTag::Butterfly ||
                    tag == dae::GameObjectTag::Boss)
                {
                    enemyCount++;
                }
            }
        }
    }

    m_TotalEnemies = enemyCount;
}