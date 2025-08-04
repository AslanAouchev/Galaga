#include "BulletComponent.h"
#include "PlayerComponent.h"
#include <HitboxComponent.h>
#include "SceneManager.h"
#include "Scene.h"
#include <random>
#include "GalagaGameManager.h"

dae::BulletComponent::BulletComponent(GameObject* pOwner, const std::string& textureFileName, BulletTag tag, float speed) :
    Component(pOwner), m_Speed{ speed }
{
    m_pTexture = std::make_unique<TextureComponent>(textureFileName, pOwner);

    GetOwner()->AddComponent<dae::HitboxComponent>(GetOwner());

	SetTag(tag);

    if (tag == BulletTag::EnemyBullet)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

        float randomValue{ dis(gen) };

        if (randomValue < -0.2f)
        {
            m_StrafeDirection = -1.0f;
        }
        else if (randomValue > 0.2f)
        {
            m_StrafeDirection = 1.0f;
        }
        else
        {
            m_StrafeDirection = 0.0f;
        }
    }

    auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
    auto& allGameObjects{ scene.GetAllGameObjects() };

	for (const auto& gameObject : allGameObjects)
	{
		if (gameObject)
		{
			auto galagamanagerComponent{ gameObject->GetComponent<GalagaGameManager>() };
			if (galagamanagerComponent)
			{
                gameObject->AddObserver(this);
			}
		}
	}
}

void dae::BulletComponent::Update(const float deltaTime)
{
    if (m_IsDestroyed)
    {
        auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
        scene.Remove(GetOwner());
        return;
    }

	if (m_IsPaused) return;

    if (m_pTexture)
    {
        m_pTexture->Update(deltaTime);
        auto currentPos{ GetOwner()->GetTransform().GetPosition() };
        
        float deltaX = m_DirectionX * m_Speed * deltaTime;
        float deltaY = m_DirectionY * m_Speed * deltaTime;
        
        if (m_BulletTag == BulletTag::EnemyBullet)
        {
            deltaX += m_StrafeDirection * (m_Speed * 0.1f) * deltaTime;
        }
        
        const float newX{ currentPos.x + deltaX };
        const float newY{ currentPos.y + deltaY };
        GetOwner()->SetPosition(newX, newY);
        
        const float screenWidth{ 640.f };
        const float ScreenHeight{ 480.f };
        if (newX < -50 || newX > screenWidth + 50 || newY < -50 || newY > ScreenHeight + 50)
        {
            m_IsDestroyed = true;
        }
        
        CheckCollisions();
    }
}

void dae::BulletComponent::Render() const
{
    if (m_IsDestroyed) return;

    if (m_pTexture)
    {
        m_pTexture->Render();
    }
}

void dae::BulletComponent::CheckCollisions()
{
    const auto ourHitbox{ GetOwner()->GetComponent<dae::HitboxComponent>() };
    if (!ourHitbox) return;

    auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
    auto& allGameObjects{ scene.GetAllGameObjects() };

    if (m_BulletTag == BulletTag::PlayerBullet)
    {
        for (const auto& gameObject : allGameObjects)
        {
            const auto targetHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto targetPlayer{gameObject->GetComponent<dae::PlayerComponent>()};

            if (targetHitbox && targetPlayer &&
                targetPlayer->GetTag() != GameObjectTag::Player && !targetPlayer->IsDead())
            {
                if (ourHitbox->IsOverlapping(targetHitbox))
                {
                    targetPlayer->TakeDamage(1);

                    Destroy();
                    return;
                }
            }
        }
    }
    else if (m_BulletTag == BulletTag::EnemyBullet)
    {
        for (const auto& gameObject : allGameObjects)
        {
            const auto targetHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto targetPlayer{ gameObject->GetComponent<dae::PlayerComponent>() };

            if (targetHitbox && targetPlayer &&
                targetPlayer->GetTag() == GameObjectTag::Player && !targetPlayer->IsDead())
            {
                if (ourHitbox->IsOverlapping(targetHitbox))
                {
                    targetPlayer->TakeDamage(1);

                    Destroy();
                    return;
                }
            }
        }
    }
}

void dae::BulletComponent::OnNotify(const EventData& event)
{
	if (event.eventType == "PauseUI")
	{
		m_IsPaused = true;
	}
	else if (event.eventType == "Resume")
	{
		m_IsPaused = false;
	}
}
