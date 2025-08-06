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
#include <BeeAiControllerComponent.h>
#include <random>
#include "InputCommands.h"
#include "InputManager.h"
#include <SDL_gamecontroller.h>
#include "BossAIControllerComponent.h"


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
        if (!m_EnteringName)
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
    else if (event.eventType == "SkipLevel")
    {
        SkipToNextLevel();
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
    UpdateScoreDisplay();
}

void GalagaGameManager::HandlePlayerKilled(const EventData&)
{
    --m_Lives;

    GetOwner()->TriggerEvent("SetLives", m_Lives - 1);

    if (m_Lives <= 0)
    {
        EndGameLose();
    }
}

void GalagaGameManager::Update(float deltaTime)
{
    if(m_LevelText)
    {
        m_LevelText->Update(deltaTime);
    }

	if (m_ScoreDisplay)
	{
		m_ScoreDisplay->Update(deltaTime);
	}

    if (m_pvpMode && m_BossDiveCooldown > 0.0f)
    {
        m_BossDiveCooldown -= deltaTime;
    }

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
            return;
        }
    }

    if (m_IsPaused)
    {
        return;
    }

    if (m_DoOnce == 0)
    {
        ++m_DoOnce;
        CreateLifeDisplay();
        CreateScoreDisplay();
        StartLevel(m_CurrentLevel);
    }

    UpdatePlayerCount();

    UpdateLevelLogic(deltaTime);

    UpdateEnemyAttacks(deltaTime);
}

void GalagaGameManager::Render() const
{
    if (m_LevelText)
    {
        m_LevelText->Render();
    }

	if (m_ScoreDisplay)
	{
		m_ScoreDisplay->Render();
	}
}

void GalagaGameManager::SetPvPMode()
{
    m_pvpMode = true;
}

void GalagaGameManager::UpdatePlayerCount()
{
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

void GalagaGameManager::UpdateLevelLogic(float deltaTime)
{
    if (!m_LevelStarted || m_AllEnemiesSpawned)
        return;

    if (m_EnemiesSpawned >= m_TotalEnemies)
    {
        m_AllEnemiesSpawned = true;
        return;
    }

    m_EnemySpawnTimer += deltaTime;
    if (m_EnemySpawnTimer >= m_EnemySpawnDelay)
    {
        SpawnNextEnemy();
        m_EnemySpawnTimer = 0.0f;
    }
}

void GalagaGameManager::UpdateEnemyAttacks(float deltaTime)
{
    if (!m_AllEnemiesSpawned)
        return;

    m_EnemyAttackTimer += deltaTime;
    if (m_EnemyAttackTimer >= m_EnemyAttackInterval)
    {
        TriggerRandomEnemyAttack();
        m_EnemyAttackTimer = 0.0f;

        float baseInterval{ 3.0f - (m_CurrentLevel * 0.5f) };
        m_EnemyAttackInterval = std::max(1.0f, baseInterval);
    }
}

void GalagaGameManager::StartLevel(int level)
{
    m_CurrentLevel = level;
    m_EnemiesKilled = 0;
    m_EnemiesSpawned = 0;
    m_LevelStarted = true;
    m_AllEnemiesSpawned = false;
    m_EnemySpawnTimer = 0.0f;

    m_BossDiveCooldown = 0.0f;

    ClearEnemies();

    SetupLevelParameters();
    UpdateScoreDisplay();
    ShowLevelStart();
}

void GalagaGameManager::SetupLevelParameters()
{
    if (m_pvpMode)
    {
        switch (m_CurrentLevel)
        {
        case 1:
            m_TotalEnemies = 6;
            m_EnemySpawnDelay = 1.2f;
            break;
        case 2:
            m_TotalEnemies = 8;
            m_EnemySpawnDelay = 1.0f;
            break;
        case 3:
            m_TotalEnemies = 10;
            m_EnemySpawnDelay = 0.8f;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (m_CurrentLevel)
        {
        case 1:
            m_TotalEnemies = 12;
            m_EnemySpawnDelay = 1.0f;
            m_EnemyAttackInterval = 3.0f;
            break;
        case 2:
            m_TotalEnemies = 16;
            m_EnemySpawnDelay = 0.8f;
            m_EnemyAttackInterval = 2.5f;
            break;
        case 3:
            m_TotalEnemies = 18;
            m_EnemySpawnDelay = 0.6f;
            m_EnemyAttackInterval = 2.0f;
            break;
        default:
            break;
        }
    }
}

void GalagaGameManager::SpawnNextEnemy()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();

    constexpr float startX{ 150.0f };
    constexpr float startY{ 50.0f };
    constexpr int columns{ 8 };

    const int row{ m_EnemiesSpawned / columns };
    const int col{ m_EnemiesSpawned % columns };

    const float spawnX{ startX + (col * 50.0f) };
    const float spawnY{ startY + (row * 40.0f) };

    auto enemy = std::make_unique<dae::GameObject>();

    if (m_pvpMode)
    {
        if (m_EnemiesSpawned == 0)
        {
            enemy->AddComponent<BossAIControllerComponent>(enemy.get());
            auto& input = dae::InputManager::GetInstance();
			input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<ShootAICOmmand>(enemy.get()), 1);
            input.BindCommand(SDL_CONTROLLER_BUTTON_B, std::make_unique<DiveAICOmmand>(enemy.get()), 1);
            input.BindCommand(SDL_CONTROLLER_BUTTON_X, std::make_unique<BeamAICOmmand>(enemy.get()), 1);
            input.BindCommand(SDL_SCANCODE_X, std::make_unique<ShootAICOmmand>(enemy.get()));
            input.BindCommand(SDL_SCANCODE_C, std::make_unique<DiveAICOmmand>(enemy.get()));
            input.BindCommand(SDL_SCANCODE_V, std::make_unique<BeamAICOmmand>(enemy.get()));
        }
		else if (m_EnemiesSpawned < 6)
		{
			enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
		}
		else if (m_EnemiesSpawned < 10)
		{
			//enemyType = dae::GameObjectTag::Butterfly;
		}
    }
    else
    {
        if (m_CurrentLevel == 1)
        {
            enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
        }
        else if (m_CurrentLevel == 2)
        {
            if (m_EnemiesSpawned < 10)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else
            {
                //enemyType = dae::GameObjectTag::Butterfly;
            }
        }
        else if (m_CurrentLevel == 3)
        {
            if (m_EnemiesSpawned < 10)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else if (m_EnemiesSpawned < 16)
            {
                //enemyType = dae::GameObjectTag::Butterfly;
            }
            else
            {
                //enemyType = dae::GameObjectTag::Boss;
            }
        }
    }

    enemy->AddObserver(GetOwner()->GetComponent<Observer>());
    GetOwner()->AddObserver(enemy->GetComponent<BaseAIController>());

    enemy->GetComponent<BaseAIController>()->SetFormationPosition(glm::vec3(spawnX, spawnY, 0.f));

    scene.Add(std::move(enemy));

    ++m_EnemiesSpawned;

    if (m_EnemiesSpawned >= m_TotalEnemies)
    {
        m_AllEnemiesSpawned = true;
    }
}

void GalagaGameManager::TriggerRandomEnemyAttack()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto& allGameObjects = scene.GetAllGameObjects();

    std::vector<dae::GameObject*> availableEnemies;

    for (const auto& gameObject : allGameObjects)
    {
        if (gameObject)
        {
            auto playerComponent{ gameObject->GetComponent<dae::PlayerComponent>() };
            if (playerComponent)
            {
                auto tag{ playerComponent->GetTag() };
                if (tag == dae::GameObjectTag::Bee ||
                    tag == dae::GameObjectTag::Butterfly ||
                    (tag == dae::GameObjectTag::Boss && !m_pvpMode))
                {
                    availableEnemies.push_back(gameObject.get());
                }
            }
        }
    }

    if (!availableEnemies.empty())
    {
        int attackersCount{ std::min(m_CurrentLevel, static_cast<int>(availableEnemies.size())) };
        attackersCount = std::max(1, attackersCount);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(availableEnemies.begin(), availableEnemies.end(), gen);

        for (int i{}; i < attackersCount; ++i)
        {
            auto aiController = availableEnemies[i]->GetComponent<BaseAIController>();
            if (aiController)
            {
                if (auto * bossAiController{ dynamic_cast<BossAIControllerComponent*>(aiController) })
                {
                    bossAiController->SetAttack(true);
                }
                else
                {
                    aiController->SetAttack(true);
                }
            }
        }
    }
}

void GalagaGameManager::ClearEnemies()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto& allGameObjects = scene.GetAllGameObjects();

    std::vector<dae::GameObject*> enemiesToRemove;

    for (const auto& gameObject : allGameObjects)
    {
        if (gameObject)
        {
            auto playerComponent{ gameObject->GetComponent<dae::PlayerComponent>() };
            if (playerComponent)
            {
                auto tag{ playerComponent->GetTag() };
                if (tag == dae::GameObjectTag::Bee ||
                    tag == dae::GameObjectTag::Butterfly ||
                    tag == dae::GameObjectTag::Boss)
                {
                    enemiesToRemove.push_back(gameObject.get());
                }
            }
        }
    }

    for (auto enemy : enemiesToRemove)
    {
        scene.Remove(enemy);
    }
}

void GalagaGameManager::ShowLevelStart()
{
    m_LevelText = std::make_unique<dae::GameObject>();
    m_LevelText->SetPosition(500, 50);

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 28);
    std::string levelString = "LEVEL " + std::to_string(m_CurrentLevel);
    m_LevelText->AddComponent<dae::TextComponent>(levelString, font, m_LevelText.get());

    ServiceLocator::getSoundSystem().play(9, 0.8f);
}

void GalagaGameManager::SkipToNextLevel()
{
    if (m_CurrentLevel < 3)
    {
        StartLevel(m_CurrentLevel + 1);
    }
    else
    {
        EndGameWin();
    }
}

void GalagaGameManager::HandleEnemyKilled(const EventData& event)
{
    ++m_EnemiesKilled;
    const int points = event.intValue;
    AddScore(points);

    CheckLevelComplete();
}

void GalagaGameManager::CheckLevelComplete()
{
    if (m_EnemiesKilled >= m_TotalEnemies)
    {
        if (m_CurrentLevel < 3)
        {
            StartLevel(m_CurrentLevel + 1);
        }
        else
        {
            EndGameWin();
        }
    }
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

void GalagaGameManager::CreateScoreDisplay()
{
    m_ScoreDisplay = std::make_unique<dae::GameObject>();
    m_ScoreDisplay->SetPosition(250.0f, 15.0f);

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
    m_ScoreDisplay->AddComponent<dae::TextComponent>("SCORE: 0", font, m_ScoreDisplay.get());
}

void GalagaGameManager::UpdateScoreDisplay()
{
    if (m_ScoreDisplay)
    {
        auto textComponent = m_ScoreDisplay->GetComponent<dae::TextComponent>();
        if (textComponent)
        {
            std::string scoreText = "SCORE: " + std::to_string(m_Score);
            textComponent->SetText(scoreText);
        }
    }
}