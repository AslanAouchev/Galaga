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

    const auto currentPos{ GetOwner()->GetTransform().GetPosition() };

    if (currentPos.y > 480.f || currentPos.x < -50.0f || currentPos.x > 680.0f)
    {
        m_DivingTimer += deltaTime;
    }

	if (m_DivingTimer >= m_MaxDiveTime)
	{
		m_DivingTimer = 0.0f;
        m_GoToFormation = true;
    }
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

	m_GoToFormation = false;

    if (!targetPlayer)
    {
        targetPlayer = GetClosestPlayer();
    }

    if (targetPlayer)
    {
        const auto playerPos{ targetPlayer->GetTransform().GetPosition() };

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> sideChoice(0.0f, 1.0f);
        std::uniform_real_distribution<float> randomOffset(-20.0f, 20.0f);

        const bool goLeft{ sideChoice(gen) < 0.5f };
        const float side{ goLeft ? -1.0f : 1.0f };

        path.push_back({ currentPos.x + (side * 40.0f), currentPos.y + 60.0f, 0 });
        path.push_back({ playerPos.x + (side * 60.0f), currentPos.y + 120.0f, 0 });
        path.push_back({ playerPos.x + (side * 30.0f), playerPos.y - 20.0f, 0 });
        path.push_back({ playerPos.x + randomOffset(gen), playerPos.y + 30.0f, 0 });

        path.push_back({ playerPos.x - (side * 40.0f), playerPos.y + 80.0f, 0 });
        path.push_back({ playerPos.x - (side * 80.0f), playerPos.y + 140.0f, 0 });

        path.push_back({ playerPos.x - (side * 120.0f), playerPos.y + 200.0f, 0 });
        path.push_back({ playerPos.x - (side * 140.0f), playerPos.y + 260.0f, 0 });
        path.push_back({ playerPos.x - (side * 120.0f), playerPos.y + 320.0f, 0 });
        path.push_back({ playerPos.x - (side * 60.0f), playerPos.y + 350.0f, 0 }); 
        path.push_back({ playerPos.x + (side * 20.0f), playerPos.y + 320.0f, 0 }); 
        path.push_back({ playerPos.x + (side * 60.0f), playerPos.y + 280.0f, 0 }); 
        path.push_back({ playerPos.x + randomOffset(gen), playerPos.y + 240.0f, 0 });
        path.push_back({ playerPos.x - (side * 40.0f), playerPos.y + 200.0f, 0 }); 

        path.push_back({ playerPos.x + (side * 150.0f), 500.0f, 0 });
    }
    else
    {
        path.push_back({ currentPos.x - 50.0f, currentPos.y + 100.0f, 0 });
        path.push_back({ currentPos.x + 50.0f, currentPos.y + 200.0f, 0 });
        path.push_back({ currentPos.x, currentPos.y + 300.0f, 0 });
        path.push_back({ currentPos.x - 100.0f, 500.0f, 0 });
    }
}

void BeeAiControllerComponent::Shoot()
{
	GetOwner()->GetComponent<dae::PlayerComponent>()->Fire(false);
}
