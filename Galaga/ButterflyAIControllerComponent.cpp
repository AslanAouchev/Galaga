#include "ButterflyAIControllerComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include <random>
#include <cmath>

ButterflyAIControllerComponent::ButterflyAIControllerComponent(dae::GameObject* owner)
    : Component(owner), BaseAIController(owner)
{
    SetSpeed(120.0f);
    SetDiveCooldown(8.0f);

    auto playerComp = owner->AddComponent<dae::PlayerComponent>(
        owner,
        "Butterfly.png",
        "BulletEnemy.png",
        1,
        200.0f
    );

    if (playerComp)
    {
        playerComp->SetBulletTag(dae::BulletTag::EnemyBullet);
        playerComp->SetTag(dae::GameObjectTag::Butterfly);
        playerComp->SetScore(80);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> intervalDis(0.2f, 0.4f);
    std::uniform_int_distribution<int> directionDis(0, 1);

    m_EvasiveInterval = intervalDis(gen);
    m_EvasiveDirection = directionDis(gen) == 0 ? -1 : 1;
}

void ButterflyAIControllerComponent::Update(const float deltaTime)
{
    BaseAIController::Update(deltaTime);

    m_EvasiveTimer += deltaTime;
    if (m_EvasiveTimer >= m_EvasiveInterval)
    {
        m_EvasiveDirection *= -1;
        m_EvasiveTimer = 0.0f;

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.15f, 0.35f);
        m_EvasiveInterval = dis(gen);
    }
}

void ButterflyAIControllerComponent::Render() const
{
}

void ButterflyAIControllerComponent::OnUpdateFormationBehavior(float deltaTime)
{
    m_FormationHover += deltaTime * 2.0f;
    const float hoverOffset{ std::sin(m_FormationHover) * 8.0f };

    glm::vec3 targetPos{ GetFormationPosition() };
    targetPos.y += hoverOffset;

    MoveTowards(targetPos, 25.0f, deltaTime);
}

void ButterflyAIControllerComponent::OnGenerateDivePath(std::vector<glm::vec3>& path)
{
    path.clear();
    const auto currentPos{ GetOwner()->GetTransform().GetPosition() };
    auto targetPlayer{ GetTargetPlayer() };

    if (!targetPlayer)
    {
        targetPlayer = GetClosestPlayer();
    }

    if (targetPlayer)
    {
        const auto playerPos{ targetPlayer->GetTransform().GetPosition() };

        path.push_back({ currentPos.x + (playerPos.x - currentPos.x) * 0.3f, currentPos.y + 40.0f, 0 });

        path.push_back({ currentPos.x - 70.0f, currentPos.y + 80.0f, 0 });
        path.push_back({ currentPos.x + 70.0f, currentPos.y + 120.0f, 0 });
        path.push_back({ currentPos.x - 50.0f, currentPos.y + 160.0f, 0 });

        path.push_back({ playerPos.x + 60.0f, currentPos.y + 200.0f, 0 });
        path.push_back({ playerPos.x - 40.0f, currentPos.y + 240.0f, 0 });
        path.push_back({ playerPos.x + 30.0f, playerPos.y - 20.0f, 0 });

        path.push_back({ playerPos.x, playerPos.y + 10.0f, 0 });

        path.push_back({ playerPos.x - 80.0f, playerPos.y + 60.0f, 0 });
        path.push_back({ playerPos.x + 100.0f, 450.0f, 0 });
        path.push_back({ currentPos.x + 120.0f, 500.0f, 0 });
    }
    else
    {
        path.push_back({ currentPos.x - 60.0f, currentPos.y + 80.0f, 0 });
        path.push_back({ currentPos.x + 60.0f, currentPos.y + 160.0f, 0 });
        path.push_back({ currentPos.x, currentPos.y + 240.0f, 0 });
        path.push_back({ currentPos.x + 80.0f, 500.0f, 0 });
    }
}

void ButterflyAIControllerComponent::Shoot()
{
    if (CanAttack())
    {
        auto playerComp{ GetOwner()->GetComponent<dae::PlayerComponent>() };
        if (playerComp)
        {
            playerComp->Fire(false);
        }
    }
}