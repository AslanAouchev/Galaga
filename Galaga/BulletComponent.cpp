#include "BulletComponent.h"
#include "PlayerComponent.h"
#include <HitboxComponent.h>
#include "SceneManager.h"
#include "Scene.h"

dae::BulletComponent::BulletComponent(GameObject* pOwner, const std::string& textureFileName, GameObjectTag tag, float speed) :
    Component(pOwner), m_Speed{ speed }
{
    m_pTexture = std::make_unique<TextureComponent>(textureFileName, pOwner);

    GetOwner()->AddComponent<dae::HitboxComponent>(GetOwner());

	SetTag(tag);
}

void dae::BulletComponent::Update(const float deltaTime)
{
    if (m_IsDestroyed)
    {
        auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
        scene.Remove(GetOwner());
        return;
    }

    if (m_pTexture)
    {
        m_pTexture->Update(deltaTime);

        auto currentPos{ GetOwner()->GetTransform().GetPosition() };
        const float newX{ currentPos.x + (m_DirectionX * m_Speed * deltaTime) };
        const float newY{ currentPos.y + (m_DirectionY * m_Speed * deltaTime) };

        GetOwner()->SetPosition(newX, newY);

        const float screenWidth{ 640.f };
        const float ScreenHeight{480.f};

        if (newX < -50 || newX > screenWidth + 50 || newY < -50 || newY > ScreenHeight + 50)
        {
            m_IsOffScreen = true;
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

    if (m_BulletTag == GameObjectTag::PlayerBullet)
    {
        for (const auto& gameObject : allGameObjects)
        {
            const auto targetHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto targetPlayer{gameObject->GetComponent<dae::PlayerComponent>()};

            if (targetHitbox && targetPlayer &&
                targetPlayer->GetTag() == GameObjectTag::Enemy)
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
    else if (m_BulletTag == GameObjectTag::EnemyBullet)
    {
        for (const auto& gameObject : allGameObjects)
        {
            const auto targetHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto targetPlayer{ gameObject->GetComponent<dae::PlayerComponent>() };

            if (targetHitbox && targetPlayer &&
                targetPlayer->GetTag() == GameObjectTag::Player)
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