#include "BeeAiControllerComponent.h"
#include "GameObject.h"
#include <cmath>
#include <PlayerComponent.h>
#include <EnemyStates.h>
#include <random>

BeeAiControllerComponent::BeeAiControllerComponent(dae::GameObject* owner)
	: Component(owner), BaseAIController(owner)
{
    SetSpeed(120.0f);
    SetDiveCooldown(0.5f);
    
    auto playerComp{ owner->AddComponent<dae::PlayerComponent>(owner, "Bee.png", "BulletEnemy.png", 1, 200.0f) };
    if (playerComp)
    {
        playerComp->SetBulletTag(dae::BulletTag::EnemyBullet);
        playerComp->SetTag(dae::GameObjectTag::Bee);
		playerComp->SetScore(50);
    }
}

BeeAiControllerComponent::~BeeAiControllerComponent()
{
}

void BeeAiControllerComponent::Update(const float deltaTime)
{
	BaseAIController::Update(deltaTime);
}

void BeeAiControllerComponent::OnUpdateFormationBehavior(float deltaTime)
{
    m_FormationWobble += deltaTime * 2.0f;
    const float wobbleOffset{ std::sin(m_FormationWobble) * 10.0f };

    glm::vec3 wobblePos{ GetFormationPosition() };
    wobblePos.x += wobbleOffset;

    MoveTowards(wobblePos, 20.0f, deltaTime);
}

void BeeAiControllerComponent::OnGenerateDivePath(std::vector<glm::vec3>& path)
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

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> randomOffset(-30.0f, 30.0f);
        const float predictedX{ playerPos.x + randomOffset(gen) };

        path.push_back({ currentPos.x + (predictedX - currentPos.x) * 0.3f, currentPos.y + 80.0f, 0 });
        path.push_back({ predictedX - 40.0f, currentPos.y + 140.0f, 0 });
        path.push_back({ predictedX, playerPos.y, 0 });
        path.push_back({ predictedX + 40.0f, playerPos.y + 60.0f, 0 });
        path.push_back({ predictedX + 100.0f, 500.0f, 0 });
    }
    else
    {
        path.push_back({ currentPos.x, currentPos.y + 150.0f, 0 });
        path.push_back({ currentPos.x + 50.0f, currentPos.y + 250.0f, 0 });
        path.push_back({ currentPos.x, 500.0f, 0 });
    }
}

void BeeAiControllerComponent::Shoot()
{
	GetOwner()->GetComponent<dae::PlayerComponent>()->Fire(false);
}
