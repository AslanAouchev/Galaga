#include "GalagaGameManager.h"
#include "GameObject.h"
#include <PlayerComponent.h>
#include <BackgroundScrollComponent.h>
#include "SceneManager.h"
#include "HighScoreManager.h"
#include "Scene.h"
#include "NameInputComponent.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "LifeIconComponent.h"

GalagaGameManager::GalagaGameManager(dae::GameObject* pOwner) : dae::Component(pOwner)
{
}

void GalagaGameManager::OnNotify(const EventData& event)
{
    if (m_ShowingGameOver && event.eventType != "GameOverComplete")
        return;

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

void GalagaGameManager::EndGameWin()
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

void GalagaGameManager::EndGameLose()
{
    GetOwner()->TriggerEvent("GameOver");

    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto gameOverObject = std::make_unique<dae::GameObject>();
    gameOverObject->SetPosition(250, 240);

    auto gameOverFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 48);
    gameOverObject->AddComponent<dae::TextComponent>("GAME OVER", gameOverFont, gameOverObject.get());

    scene.Add(std::move(gameOverObject));

    m_ShowingGameOver = true;
    m_GameOverTimer = m_GameOverDuration;

    ServiceLocator::getSoundSystem().stopAllSounds();

    ServiceLocator::getSoundSystem().play(8, 0.8f);
}

void GalagaGameManager::AddScore(int points)
{
    m_Score += points;
}

void GalagaGameManager::HandlePlayerKilled(const EventData& )
{
    --m_Lives;

    GetOwner()->TriggerEvent("SetLives", m_Lives - 1);

    if (m_Lives <= 0)
    {
        EndGameLose();
    }
}

void GalagaGameManager::HandleEnemyKilled(const EventData& event)
{
    ++m_EnemiesKilled;
    const int points{ event.intValue };
    AddScore(points);

    CheckLevelComplete();
}

void GalagaGameManager::CheckLevelComplete()
{
    if (m_EnemiesKilled >= m_TotalEnemies)
    {
        EndGameWin();
    }
}

void GalagaGameManager::Update(float deltaTime)
{
    if (m_ShowingGameOver)
    {
        m_GameOverTimer -= deltaTime;
        if (m_GameOverTimer <= 0.0f)
        {
            m_ShowingGameOver = false;

            extern void loadMainMenu();
            loadMainMenu();
            auto& sceneManager = dae::SceneManager::GetInstance();
            sceneManager.SetActiveScene("MainMenu");
        }
    }

    static int DoOnce = 0;
    if (DoOnce == 0)
    {
		++DoOnce;
        CreateLifeDisplay();
    }

    static int previousObserverCount = 0;
    int currentObserverCount{ GetOwner()->GetObserverCount() };

    if (currentObserverCount > previousObserverCount)
    {
		previousObserverCount = currentObserverCount;
        auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
        auto& allGameObjects = scene.GetAllGameObjects();

        int playerCount{};

        for (const auto& gameObject : allGameObjects)
        {
            if (gameObject)
            {
                auto playerComponent{ gameObject->GetComponent<dae::PlayerComponent>() };
                if (playerComponent && playerComponent->GetTag() == dae::GameObjectTag::Player)
                {
                    playerCount++;
                }
            }
        }

		m_PlayerCount = playerCount;

        if (m_PlayerCount == 1)
        {
			m_Lives = 4;
		}
		else if (m_PlayerCount == 2)
		{
			m_Lives = 5;
		}

        GetOwner()->TriggerEvent("SetLives", m_Lives - 1);
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
                auto tag{ playerComponent->GetTag() };
                if (tag == dae::GameObjectTag::Bee ||
                    tag == dae::GameObjectTag::Butterfly ||
                    tag == dae::GameObjectTag::Boss)
                {
                    ++enemyCount;
                }
            }
        }
    }

    m_TotalEnemies = enemyCount;
}

void GalagaGameManager::CreateLifeDisplay()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();

    for (int i{}; i < 5; ++i)
    {
        auto lifeIcon = std::make_unique<dae::GameObject>();
        lifeIcon->SetPosition(static_cast<float>(20 + (i * 35)), 450.f);
        lifeIcon->AddComponent<dae::TextureComponent>("galaga.png", lifeIcon.get());
        lifeIcon->AddComponent<LifeIconComponent>(lifeIcon.get(), i);

        GetOwner()->AddObserver(lifeIcon->GetComponent<LifeIconComponent>());

        scene.Add(std::move(lifeIcon));
    }
}