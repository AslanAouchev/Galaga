#include "GameObject.h"
#include <iostream>
#include <cmath>
#include <PlayerComponent.h>
#include <random>
#include "BossAIControllerComponent.h"
#include <EnemyStates.h>
#include "SceneManager.h"
#include "Scene.h"

BossAIControllerComponent::BossAIControllerComponent(dae::GameObject* owner)
    : Component(owner), BaseAIController(owner)
{
    SetSpeed(100.0f);
    SetDiveCooldown(10.f);

    auto playerComp{ owner->AddComponent<dae::PlayerComponent>(owner, "Boss.png", "BulletEnemy.png", 2, 200.0f) };
    if (playerComp)
    {
        playerComp->SetBulletTag(dae::BulletTag::EnemyBullet);
        playerComp->SetTag(dae::GameObjectTag::Boss);
        playerComp->SetScore(150);
    }
}

void BossAIControllerComponent::Update(const float deltaTime)
{
	BaseAIController::Update(deltaTime);

    UpdateTractorBeam(deltaTime);

    if (m_EnemyPlayerComponent)
    {
        m_EnemyPlayerComponent->Update(deltaTime);
    }

    if (m_EnemyPlayerComponent && m_EnemyPlayerComponent->GetOwner() && !m_EnemyPlayerComponent->GetOwner()->IsActive())
    {
        m_EnemyPlayerComponent = nullptr;
    }
}

void BossAIControllerComponent::Render() const
{
    if(m_EnemyPlayerComponent)
    {
        m_EnemyPlayerComponent->Render();
    }
}

void BossAIControllerComponent::StartTractorBeam()
{
    bool hasFighter{ (m_EnemyPlayerComponent != nullptr) };

    if (CanAttack() || hasFighter) return;

    SetAttack(true);

    auto targetPlayer{ GetClosestPlayer() };
    if (targetPlayer)
    {
        m_TractorBeamTarget = targetPlayer;
        m_TractorBeamActive = true;
        m_TractorBeamTimer = m_TractorBeamDuration;
    }
}

void BossAIControllerComponent::ShootTractorBeam()
{
    bool hasFighter{ (m_EnemyPlayerComponent != nullptr) };
	if (hasFighter)
	{
		return;
	}

    if (m_TractorBeamActive)
    {
        m_IsTractorBeamRun = true;
    }
}

void BossAIControllerComponent::SetAttack(bool SetAttack)
{
    if (m_TractorBeamActive)
    {
        return;
    }

    bool hasFighter{ (m_EnemyPlayerComponent != nullptr) };

    if (SetAttack && !hasFighter)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        m_TractorBeamActive = dis(gen) < 0.4f;
    }

    BaseAIController::SetAttack(SetAttack);
}

void BossAIControllerComponent::CreateGalagaEnemyPlayer()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto& allObjects = scene.GetAllGameObjects();

    for (auto& obj : allObjects)
    {
        if (obj->GetComponent<TractorBeamComponent>())
        {
            scene.Remove(obj.get());
        }
    }

    m_IsTractorBeamRun = false;
    m_TractorBeamActive = false;
    m_DoOnce = false;
    SetAttack(false);

    auto enemyPlayer{ std::make_unique<dae::GameObject>() };
    enemyPlayer->AddComponent<EnemyPlayerAIComponent>(enemyPlayer.get(), this);

	m_EnemyPlayerComponent = enemyPlayer->GetComponent<EnemyPlayerAIComponent>();

    scene.Add(std::move(enemyPlayer));
}

void BossAIControllerComponent::UpdateTractorBeam(float deltaTime)
{
    if (m_IsTractorBeamRun)
    {
        m_TractorBeamTimer -= deltaTime;

        if (!m_DoOnce)
        {
            auto tractorBeapObject{ std::make_unique<dae::GameObject>() };

            tractorBeapObject->AddComponent<TractorBeamComponent>(tractorBeapObject.get(), this);
            tractorBeapObject->SetPosition(GetOwner()->GetTransform().GetPosition().x - 30.f,
                GetOwner()->GetTransform().GetPosition().y + 35.f);

            m_DoOnce = true;

            auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
            scene.Add(std::move(tractorBeapObject));
        }

        if (m_TractorBeamTimer <= 0.0f)
        {
            auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
            auto& allObjects = scene.GetAllGameObjects();

            for (auto& obj : allObjects)
            {
                if (obj->GetComponent<TractorBeamComponent>())
                {
                    scene.Remove(obj.get());
                }
            }
            m_IsTractorBeamRun = false;
			m_TractorBeamActive = false;
            m_DoOnce = false;
            SetAttack(false);
        }
    }
}

void BossAIControllerComponent::OnUpdateFormationBehavior(float deltaTime)
{
    m_formationWobble += deltaTime * 0.6f;
    const float wobbleOffset{ std::sin(m_formationWobble) * 25.0f };

    glm::vec3 wobblePos{ GetFormationPosition() };
    wobblePos.x += wobbleOffset;

    MoveTowards(wobblePos, 12.0f, deltaTime);
}

void BossAIControllerComponent::OnGenerateDivePath(std::vector<glm::vec3>& path)
{
    path.clear();
    const auto currentPos{ GetOwner()->GetTransform().GetPosition() };
    auto targetPlayer{ GetTargetPlayer() };

    if (!targetPlayer)
    {
        targetPlayer = GetClosestPlayer();
    }

    if (m_IsTractorBeamRun && targetPlayer)
    {
        const auto playerPos{ targetPlayer->GetTransform().GetPosition() };

        path.push_back({ currentPos.x - 80.0f, currentPos.y + 20.0f, 0 });
        path.push_back({ currentPos.x - 100.0f, currentPos.y, 0 });
        path.push_back({ currentPos.x - 80.0f, currentPos.y - 20.0f, 0 });
        path.push_back({ currentPos.x, currentPos.y - 20.0f, 0 });
        path.push_back({ currentPos.x + 80.0f, currentPos.y - 20.0f, 0 });
        path.push_back({ currentPos.x + 100.0f, currentPos.y, 0 });
        path.push_back({ currentPos.x + 80.0f, currentPos.y + 20.0f, 0 });
        path.push_back({ currentPos.x, currentPos.y, 0 });

        path.push_back({ playerPos.x, 240.0f, 0 });

        path.push_back({ playerPos.x, 240.0f, 0 });

        path.push_back({ playerPos.x + 100.0f, 500.0f, 0 });
    }
    else if (targetPlayer)
    {
        const auto playerPos{ targetPlayer->GetTransform().GetPosition() };

        path.push_back({ currentPos.x + (playerPos.x - currentPos.x) * 0.4f, currentPos.y + 70.0f, 0 });
        path.push_back({ playerPos.x - 60.0f, currentPos.y + 130.0f, 0 });
        path.push_back({ playerPos.x, playerPos.y - 30.0f, 0 });
        path.push_back({ playerPos.x + 60.0f, playerPos.y + 80.0f, 0 });
        path.push_back({ playerPos.x + 120.0f, 500.0f, 0 });
    }
    else
    {
        path.push_back({ currentPos.x, currentPos.y + 100.0f, 0 });
        path.push_back({ currentPos.x + 80.0f, 500.0f, 0 });
    }
}

void BossAIControllerComponent::Shoot()
{
	if (!m_IsTractorBeamRun && !m_TractorBeamActive && CanAttack())
    {
        GetOwner()->GetComponent<dae::PlayerComponent>()->Fire(false);
    }
}

void BossAIControllerComponent::ShootFighter()
{
    if (m_EnemyPlayerComponent)
    {
		m_EnemyPlayerComponent->Shoot();
    }
}
