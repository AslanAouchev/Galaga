#include "TractorBeamComponent.h"
#include "GameObject.h"
#include "PlayerComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include <iostream>
#include "BossAIControllerComponent.h"

TractorBeamComponent::TractorBeamComponent(dae::GameObject* owner, BossAIControllerComponent* bossController)
    : Component(owner), m_BossController(bossController)
{
    m_TextureComponent = std::make_unique<dae::TextureComponent>("BossBeam.png", GetOwner());

    if (m_TextureComponent)
    {
        auto textureSize{ m_TextureComponent->GetTextureSize() };
        m_BeamWidth =  static_cast<float>(textureSize.x);
        m_BeamHeight = static_cast<float>(textureSize.y);
    }
}

void TractorBeamComponent::Update(const float deltaTime)
{
    if (!m_HasHitPlayer)
    {
        CheckPlayerCollisions();
    }

	if (m_TextureComponent)
	{
		m_TextureComponent->Update(deltaTime);
	}
}

void TractorBeamComponent::Render() const
{
    if (m_TextureComponent)
    {
        m_TextureComponent->Render();
    }
}

bool TractorBeamComponent::IsPositionInBeam(const glm::vec3& position) const
{
    auto beamPos{ GetOwner()->GetTransform().GetPosition() };

    float left{ beamPos.x - m_BeamWidth * 0.5f };
    float right{ beamPos.x + m_BeamWidth * 0.5f };
    float top{ beamPos.y - m_BeamHeight * 0.5f };
    float bottom{ beamPos.y + m_BeamHeight * 0.5f };

    return (position.x >= left && position.x <= right &&
        position.y >= top && position.y <= bottom);
}

void TractorBeamComponent::CheckPlayerCollisions()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();

    auto& gameObjects = scene.GetAllGameObjects();

    for (const auto& obj : gameObjects)
    {
        auto playerComp{ obj->GetComponent<dae::PlayerComponent>() };
        if (playerComp && playerComp->GetTag() == dae::GameObjectTag::Player)
        {
            if (IsPositionInBeam(obj->GetTransform().GetPosition()))
            {
				playerComp->TakeDamage(1);
                OnPlayerHit();
                break;
            }
        }
    }
}

void TractorBeamComponent::OnPlayerHit()
{
    if (m_HasHitPlayer) return;

    m_HasHitPlayer = true;

    if (m_BossController)
    {
        m_BossController->CreateGalagaEnemyPlayer();
    }
}