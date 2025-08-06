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
#include "ButterflyAIControllerComponent.h"

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
    gameOverObject->SetPosition(200, 180);
    auto gameOverFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 48);
    gameOverObject->AddComponent<dae::TextComponent>("GAME OVER", gameOverFont, gameOverObject.get());
    scene.Add(std::move(gameOverObject));

    const int missedShots{ m_ShotAmount - m_EnemiesKilled };
    float accuracy{};
    if (m_ShotAmount > 0)
    {
        accuracy = (static_cast<float>(m_EnemiesKilled) / static_cast<float>(m_ShotAmount)) * 100.0f;
    }

    auto statsFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);

    auto shotsObject = std::make_unique<dae::GameObject>();
    shotsObject->SetPosition(200, 250);
    shotsObject->AddComponent<dae::TextComponent>("SHOTS FIRED: " + std::to_string(m_ShotAmount), statsFont, shotsObject.get());
    scene.Add(std::move(shotsObject));

    auto hitsObject = std::make_unique<dae::GameObject>();
    hitsObject->SetPosition(200, 280);
    hitsObject->AddComponent<dae::TextComponent>("HITS: " + std::to_string(m_EnemiesKilled), statsFont, hitsObject.get());
    scene.Add(std::move(hitsObject));

    auto missesObject = std::make_unique<dae::GameObject>();
    missesObject->SetPosition(200, 310);
    missesObject->AddComponent<dae::TextComponent>("MISSES: " + std::to_string(missedShots), statsFont, missesObject.get());
    scene.Add(std::move(missesObject));

    auto accuracyObject = std::make_unique<dae::GameObject>();
    accuracyObject->SetPosition(200, 340);
    std::string accuracyText = "ACCURACY: " + std::to_string(static_cast<int>(accuracy)) + "%";
    accuracyObject->AddComponent<dae::TextComponent>(accuracyText, statsFont, accuracyObject.get());
    scene.Add(std::move(accuracyObject));

    auto scoreObject = std::make_unique<dae::GameObject>();
    scoreObject->SetPosition(200, 380);
    scoreObject->AddComponent<dae::TextComponent>("FINAL SCORE: " + std::to_string(m_Score), statsFont, scoreObject.get());
    scene.Add(std::move(scoreObject));

    m_ShowingGameOver = true;

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

    UpdatePlayerCount();

    if (m_DoOnce == 0)
    {
        ++m_DoOnce;
        CreateLifeDisplay();
        CreateScoreDisplay();
        StartLevel(m_CurrentLevel);

        GetOwner()->TriggerEvent("SetLives", m_Lives - 1);
    }

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
    int currentObserverCount{ GetOwner()->GetObserverCount() };

    if (currentObserverCount > m_PreviousObserverCount)
    {
        m_PreviousObserverCount = currentObserverCount;
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

        if (!m_LivesInitialized)
        {
            if (m_PlayerCount == 1)
            {
                m_Lives = 4;
            }
            else if (m_PlayerCount == 2)
            {
                m_Lives = 5;
            }

            GetOwner()->TriggerEvent("SetLives", m_Lives - 1);
            m_LivesInitialized = true;
        }
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

    const int currentTeam{ m_EnemiesSpawned / 4 };
    const int nextTeamStartIndex{ currentTeam * 4 };

    if (m_EnemiesSpawned == nextTeamStartIndex)
    {
        m_EnemySpawnTimer += deltaTime;
        if (m_EnemySpawnTimer >= m_EnemySpawnDelay)
        {
            const int enemiesToSpawn{ std::min(4, m_TotalEnemies - m_EnemiesSpawned) };

            for (int i{}; i < enemiesToSpawn; ++i)
            {
                SpawnNextEnemy();
            }
            m_EnemySpawnTimer = 0.0f;
        }
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
            m_TotalEnemies = 9;
            break;
        case 2:
            m_TotalEnemies = 9;
            break;
        case 3:
            m_TotalEnemies = 9;
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
            m_TotalEnemies = 32;
            m_EnemyAttackInterval = 3.0f;
            break;
        case 2:
            m_TotalEnemies = 40;
            m_EnemyAttackInterval = 2.5f;
            break;
        case 3:
            m_TotalEnemies = 48;
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

    const float formationX{ startX + (col * 50.0f) };
    const float formationY{ startY + (row * 40.0f) };

    auto enemy = std::make_unique<dae::GameObject>();

    glm::vec3 entrancePos;
    std::vector<glm::vec3> entrancePath;

    const int teamId{ m_EnemiesSpawned / 4 };
    const int positionInTeam{ m_EnemiesSpawned % 4 };
    const float screenWidth{ 640.f };

    if (teamId % 3 == 0)
    {
        const float teamSpacing{ 60.0f };
        const float teamStartX{ -150.0f - (positionInTeam * teamSpacing) };

        entrancePos = { teamStartX, formationY - 120.0f, 0.0f };

        entrancePath.push_back({ teamStartX + 100.0f, formationY - 100.0f, 0.0f });
        entrancePath.push_back({ teamStartX + 200.0f, formationY - 60.0f, 0.0f });
        entrancePath.push_back({ formationX - 40.0f, formationY - 30.0f, 0.0f });
        entrancePath.push_back({ formationX, formationY, 0.0f });
    }
    else if (teamId % 3 == 1)
    {
        const float teamSpacing{ 60.0f };
        const float teamStartX{ screenWidth + 150.0f + (positionInTeam * teamSpacing) };

        entrancePos = { teamStartX, formationY - 120.0f, 0.0f };

        entrancePath.push_back({ teamStartX - 100.0f, formationY - 100.0f, 0.0f });
        entrancePath.push_back({ teamStartX - 200.0f, formationY - 60.0f, 0.0f });
        entrancePath.push_back({ formationX + 40.0f, formationY - 30.0f, 0.0f });
        entrancePath.push_back({ formationX, formationY, 0.0f });
    }
    else
    {
        const float teamSpacing{ 50.0f };
        const float offsetX{ (positionInTeam % 2 == 0) ? -teamSpacing : teamSpacing };
        const float offsetY{ (positionInTeam / 2) * 40.0f };

        entrancePos = { formationX + offsetX, -80.0f - offsetY, 0.0f };

        entrancePath.push_back({ formationX + (offsetX * 0.7f), 50.0f + offsetY, 0.0f });
        entrancePath.push_back({ formationX + (offsetX * 0.4f), formationY - 80.0f, 0.0f });
        entrancePath.push_back({ formationX + (offsetX * 0.2f), formationY - 40.0f, 0.0f });
        entrancePath.push_back({ formationX, formationY, 0.0f });
    }

    enemy->SetPosition(entrancePos.x, entrancePos.y);

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

            enemy->GetComponent<BaseAIController>()->SetFormationPosition(glm::vec3(350.0f, 50.0f, 0.0f));
        }
        else
        {
            const int adjustedIndex{ m_EnemiesSpawned - 1 };
            const int adjustedRow{ adjustedIndex / (columns - 2) + 1 };
            const int adjustedCol{ (adjustedIndex % (columns - 2)) + 1 };

            const float adjustedX{startX + (adjustedCol * 50.0f)};
            const float adjustedY{startY + (adjustedRow * 40.0f)};

            if (m_CurrentLevel == 1)
            {
                if (row >= 2)
                {
                    enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
                }
                else
                {
                    enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
                }
            }
            else if (m_CurrentLevel == 2)
            {
                if (adjustedRow <= 1)
                {
                    enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
                }
                else
                {
                    enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
                }
            }
            else if (m_CurrentLevel == 3)
            {
                if (adjustedRow <= 2)
                {
                    enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
                }
                else
                {
                    enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
                }
            }

            enemy->GetComponent<BaseAIController>()->SetFormationPosition(glm::vec3(adjustedX, adjustedY, 0.0f));
        }
    }
    else
    {
        if (m_CurrentLevel == 1)
        {
            if (row >= 3)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else if (row >= 2)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else
            {
                enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
            }
        }
        else if (m_CurrentLevel == 2)
        {
            if (row >= 4)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else if (row >= 2)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else
            {
                if (m_EnemiesSpawned < 8)
                {
                    enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
                }
                else
                {
                    if ((col % 2) == 0)
                    {
                        enemy->AddComponent<BossAIControllerComponent>(enemy.get());
                    }
                    else
                    {
                        enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
                    }
                }
            }
        }
        else if (m_CurrentLevel == 3)
        {
            if (row >= 4)
            {
                enemy->AddComponent<BeeAiControllerComponent>(enemy.get());
            }
            else if (row >= 2)
            {
                enemy->AddComponent<ButterflyAIControllerComponent>(enemy.get());
            }
            else
            {
                enemy->AddComponent<BossAIControllerComponent>(enemy.get());
            }
        }
    }

    enemy->GetComponent<BaseAIController>()->SetFormationPosition(glm::vec3(formationX, formationY, 0.f));

    if (auto baseAI{ enemy->GetComponent<BaseAIController>() })
    {
        baseAI->SetEntrancePath(entrancePath);
    }

    enemy->AddObserver(GetOwner()->GetComponent<Observer>());
    GetOwner()->AddObserver(enemy->GetComponent<BaseAIController>());

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
    std::vector<dae::GameObject*> availableBosses;
    std::vector<dae::GameObject*> availableEscorts;

    for (const auto& gameObject : allGameObjects)
    {
        if (gameObject && gameObject->IsActive() && !gameObject->IsMarkedForDestruction())
        {
            auto playerComponent{ gameObject->GetComponent<dae::PlayerComponent>() };
            auto aiController{ gameObject->GetComponent<BaseAIController>() };

            if (playerComponent && aiController && !aiController->CanAttack())
            {
                auto tag{ playerComponent->GetTag() };
                if (tag == dae::GameObjectTag::Boss && !m_pvpMode)
                {
                    availableBosses.push_back(gameObject.get());
                    availableEnemies.push_back(gameObject.get());
                }
                else if (tag == dae::GameObjectTag::Bee || tag == dae::GameObjectTag::Butterfly)
                {
                    availableEscorts.push_back(gameObject.get());
                    availableEnemies.push_back(gameObject.get());
                }
            }
        }
    }

    if (availableEnemies.empty())
    {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(availableEnemies.begin(), availableEnemies.end(), gen);

    bool bossAttacking{};
    BossAIControllerComponent* attackingBoss{ nullptr };

    int attackersCount{ std::min(m_CurrentLevel, static_cast<int>(availableEnemies.size())) };
    attackersCount = std::max(1, attackersCount);

    for (auto* enemy : availableEnemies)
    {
        auto aiController{ enemy->GetComponent<BaseAIController>() };
        if (aiController)
        {
            if (auto * bossAiController{ dynamic_cast<BossAIControllerComponent*>(aiController) })
            {
                bossAttacking = true;
                attackingBoss = bossAiController;
                bossAiController->SetAttack(true);
                break;
            }
        }
    }

    if (bossAttacking && attackingBoss && !attackingBoss->IsTractorBeamActive())
    {
        availableEscorts.erase(
            std::remove_if(availableEscorts.begin(), availableEscorts.end(),
                [](dae::GameObject* escort) {
                    return !escort || !escort->IsActive() || escort->IsMarkedForDestruction() ||
                        !escort->GetComponent<BaseAIController>() ||
                        escort->GetComponent<BaseAIController>()->CanAttack();
                }),
            availableEscorts.end());

        std::shuffle(availableEscorts.begin(), availableEscorts.end(), gen);

        int escortCount{ std::min(m_CurrentLevel, static_cast<int>(availableEscorts.size())) };
        escortCount = std::min(escortCount, 3);

        int successfulEscorts{};
        for (int i{}; i < availableEscorts.size() && successfulEscorts < escortCount; ++i)
        {
            auto escortAI{ availableEscorts[i]->GetComponent<BaseAIController>() };
            if (escortAI && !escortAI->CanAttack())
            {
                escortAI->SetAttack(true);
                successfulEscorts++;
            }
        }
    }
    else if (!bossAttacking)
    {
        int successfulAttackers{};
        for (int i{}; i < availableEnemies.size() && successfulAttackers < attackersCount; ++i)
        {
            auto aiController{ availableEnemies[i]->GetComponent<BaseAIController>() };
            if (aiController && !aiController->CanAttack())
            {
                aiController->SetAttack(true);
                successfulAttackers++;
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
    m_LevelText->SetPosition(535, 0);

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
