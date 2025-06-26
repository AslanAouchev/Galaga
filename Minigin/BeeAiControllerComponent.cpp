#include "BeeAiControllerComponent.h"
#include "GameObject.h"
#include <cmath>
#include "Scene.h"
#include "SceneManager.h"
#include <PlayerComponent.h>

BeeAiControllerComponent::BeeAiControllerComponent(dae::GameObject* owner)
    : Component(owner), m_AICore(std::make_unique<BaseAIController>(owner))
{
    m_AICore->SetSpeed(50.0f);
    m_AICore->SetDiveCooldown(6.0f);

    m_AICore->OnUpdateFormationBehavior = [this](float deltaTime) { UpdateFormationBehavior(deltaTime); };
    m_AICore->OnGenerateDivePath = [this](std::vector<glm::vec3>& path) { GenerateDivePath(path); };
    m_AICore->OnShouldDive = [this]() { return ShouldDive(); };
    m_AICore->Shoot = [this]() {return Shoot();};
}

void BeeAiControllerComponent::Update(const float deltaTime)
{
    if(m_ExistenceTimer < 1.0f)
    {
        m_ExistenceTimer += deltaTime;
    }

    if (!m_HasFoundPlayers && m_ExistenceTimer >= 1.0f)
    {
        UpdatePlayersFromScene();
        m_HasFoundPlayers = true;
    }

    m_AICore->Update(deltaTime);
}

void BeeAiControllerComponent::UpdatePlayersFromScene()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
    auto& allGameObjects = scene.GetAllGameObjects();

    std::vector<dae::GameObject*> foundPlayers;

    for (auto& gameObject : allGameObjects)
    {
        if (gameObject && !gameObject->IsMarkedForDestruction())
        {
            auto playerComponent = gameObject->GetComponent<dae::PlayerComponent>();
            if (playerComponent && playerComponent->GetTag() == dae::GameObjectTag::Player)
            {
                foundPlayers.push_back(gameObject.get());
            }
        }
    }

    m_AICore->SetPlayers(foundPlayers);
}


void BeeAiControllerComponent::UpdateFormationBehavior(float deltaTime)
{
    m_FormationWobble += deltaTime * 2.0f;
    const float wobbleOffset{ std::sin(m_FormationWobble) * 10.0f };

    glm::vec3 wobblePos{ m_AICore->GetFormationPosition() };
    wobblePos.x += wobbleOffset;

    m_AICore->MoveTowards(wobblePos, 20.0f, deltaTime);
}

void BeeAiControllerComponent::GenerateDivePath(std::vector<glm::vec3>& path)
{
    path.clear();
    const auto currentPos{ GetOwner()->GetTransform().GetPosition() };

    auto targetPlayer{ m_AICore->GetTargetPlayer() };
    if (!targetPlayer)
    {
        targetPlayer = m_AICore->GetClosestPlayer();
    }

    if (targetPlayer)
    {
        const auto playerPos{ targetPlayer->GetTransform().GetPosition() };
        path.push_back({ currentPos.x, currentPos.y + 100.0f, 0 });
        path.push_back({ playerPos.x, currentPos.y + 200.0f, 0 });
        path.push_back({ playerPos.x, 500.0f, 0 });
    }
    else
    {
        path.push_back({ currentPos.x, currentPos.y + 200.0f, 0 });
        path.push_back({ currentPos.x, 500.0f, 0 });
    }
}

bool BeeAiControllerComponent::ShouldDive()
{
    return !m_AICore->GetPlayers().empty();
}

void BeeAiControllerComponent::Shoot()
{
	//GetOwner()->GetComponent<dae::PlayerComponent>()->Fire();
}
