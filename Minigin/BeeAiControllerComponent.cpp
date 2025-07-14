#include "BeeAiControllerComponent.h"
#include "GameObject.h"
#include <cmath>
#include "Scene.h"
#include "SceneManager.h"
#include <PlayerComponent.h>

BeeAiControllerComponent::BeeAiControllerComponent(dae::GameObject* owner)
	: Component(owner), BaseAIController(owner)
{
    SetSpeed(120.0f);
    SetDiveCooldown(0.5f);
}

void BeeAiControllerComponent::Update(const float deltaTime)
{
    if (m_Paused || m_KilledPaused) return;

	BaseAIController::Update(deltaTime);

    if(m_ExistenceTimer < 1.0f)
    {
        m_ExistenceTimer += deltaTime;
    }

    if (!m_HasFoundPlayers && m_ExistenceTimer >= 1.0f)
    {
        UpdatePlayersFromScene();
        m_HasFoundPlayers = true;
    }
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

    SetPlayers(foundPlayers);
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

bool BeeAiControllerComponent::OnShouldDive()
{
    return !GetPlayers().empty();
}

void BeeAiControllerComponent::Shoot()
{
	//GetOwner()->GetComponent<dae::PlayerComponent>()->Fire();
}
