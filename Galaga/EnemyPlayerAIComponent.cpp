#include "EnemyPlayerAIComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include <random>
#include "BossAIControllerComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GalagaGameManager.h"

EnemyPlayerAIComponent::EnemyPlayerAIComponent(dae::GameObject* owner, BossAIControllerComponent* boss)
    : Component(owner), m_Boss(boss)
{
    auto playerComp = GetOwner()->AddComponent<dae::PlayerComponent>(
        GetOwner(),
        "GalagaEnemy.png",
        "BulletEnemy.png",
        1,
        100.0f
    );

    if (playerComp)
    {
        playerComp->SetBulletTag(dae::BulletTag::EnemyBullet);
        playerComp->SetTag(dae::GameObjectTag::EnemyPlayer);
        playerComp->SetScore(0);
    }

    if (m_Boss && m_Boss->GetOwner())
    {
        auto bossPos{ m_Boss->GetOwner()->GetTransform().GetPosition() };
        GetOwner()->SetPosition(bossPos.x - 60.0f, bossPos.y);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(1.5f, 3.0f);
    m_ShootInterval = dis(gen);

    auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
    auto& allGameObjects{ scene.GetAllGameObjects() };

    for (const auto& gameObject : allGameObjects)
    {
        if (auto * ga{ gameObject->GetComponent<GalagaGameManager>() })
        {
            ga->GetOwner()->AddObserver(this);
        }
    }
}

void EnemyPlayerAIComponent::Update(const float deltaTime)
{
    if (m_Boss && m_Boss->GetOwner())
    {
        auto bossPos{ m_Boss->GetOwner()->GetTransform().GetPosition() };
        GetOwner()->SetPosition(bossPos.x - 45.0f, bossPos.y);
    }

    m_ShootTimer += deltaTime;
    if (m_ShootTimer >= m_ShootInterval && !m_PlayerControlled)
    {
        if (!m_DontShoot)
        {
            auto playerComp{ GetOwner()->GetComponent<dae::PlayerComponent>() };
            if (playerComp)
            {
                playerComp->Fire(false);
            }
        }

        m_ShootTimer = 0.0f;

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(1.5f, 3.5f);
        m_ShootInterval = dis(gen);
    }

    if (!m_Boss->GetOwner()->IsActive())
    {
        auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
        scene.Remove(GetOwner());
    }
}

void EnemyPlayerAIComponent::Shoot()
{
    m_PlayerControlled = true;

    if (m_ShootTimer >= m_ShootInterval)
    {
        auto playerComp{ GetOwner()->GetComponent<dae::PlayerComponent>() };
        if (playerComp && !m_DontShoot)
        {
            playerComp->Fire(false);
            m_ShootTimer = 0.0f;
        }
    }
}

void EnemyPlayerAIComponent::OnNotify(const EventData& event)
{
    if (event.eventType == "ManagerPlayerHit")
    {
        m_DontShoot = true;
    }
    else if (event.eventType == "ResumeKilled")
    {
        m_DontShoot = false;
    }
    else if (event.eventType == "PauseUI" || event.eventType == "GameOver")
    {
        m_DontShoot = true;
    }
    else if (event.eventType == "Resume")
    {
        m_DontShoot = false;
    }
}
