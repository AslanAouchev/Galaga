#include "PlayerComponent.h"
#include "../Galaga/BulletComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "HitboxComponent.h"
#include <SDLSoundSystem.h>

dae::PlayerComponent::PlayerComponent(GameObject* pOwner, const std::string& textureFileName, const std::string& bulletTextureFileName, int health, float bulletSpeed,
    GameObjectTag tag, GameObjectTag bulletTag) :
    Component(pOwner), m_Health{ health }, m_BulletString{ bulletTextureFileName }, m_Tag{ tag }, m_BulletTag{ bulletTag }, m_BulletSpeed{bulletSpeed}
{
    m_pTexture = std::make_unique<TextureComponent>(textureFileName, pOwner);
	GetOwner()->AddComponent<dae::HitboxComponent>(GetOwner());
}

void dae::PlayerComponent::Update(const float deltaTime)
{
    if (m_pTexture)
    {
        m_pTexture->Update(deltaTime);
    }

    if (m_FireCooldown > 0.0f)
    {
        m_FireCooldown -= deltaTime;
    }

    if (m_IsDead)
    {
        auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
        scene.Remove(GetOwner());
        return;
    }

    CheckCollisions();
}

void dae::PlayerComponent::Render() const
{
	if (m_pTexture)
    {
        m_pTexture->Render();
    }
}

void dae::PlayerComponent::TakeDamage(int amount)
{
    m_Health -= amount;
    ServiceLocator::getSoundSystem().play(5, 0.8f);
    if (m_Health <= 0)
    {
        m_Health = 0;
        m_IsDead = true;
    }
}

bool dae::PlayerComponent::Fire()
{
    if (m_FireCooldown <= 0.0f)
    {
        auto bullet{ std::make_unique<dae::GameObject>() };

        const auto playerPos{ GetOwner()->GetTransform().GetPosition() };

        bullet->SetPosition(playerPos.x + 16, playerPos.y - 10);

        bullet->AddComponent<dae::BulletComponent>(bullet.get(), m_BulletString, m_BulletTag, m_BulletSpeed);

        auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
		scene.Add(std::move(bullet));

        m_FireCooldown = m_MaxFireCooldown;
        return true;
    }
    return false;
}

glm::vec2 dae::PlayerComponent::GetTextureSize() const
{
    return m_pTexture->GetTextureSize();
}

void dae::PlayerComponent::CheckCollisions()
{
    auto ourHitbox{ GetOwner()->GetComponent<dae::HitboxComponent>() };
    if (!ourHitbox) return;

    auto& scene{ dae::SceneManager::GetInstance().GetActiveScene() };
    auto& allGameObjects{ scene.GetAllGameObjects() };

    if (m_Tag == GameObjectTag::Player)
    {
        for (const auto& gameObject : allGameObjects)
        {
            if (gameObject.get() == GetOwner()) continue;

            const auto otherHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto otherPlayer{gameObject->GetComponent<dae::PlayerComponent>()};

            if (otherHitbox && otherPlayer)
            {
                if (otherPlayer->GetTag() == GameObjectTag::Enemy)
                {
                    if (ourHitbox->IsOverlapping(otherHitbox))
                    {
                        TakeDamage(1);
                        return;
                    }
                }
            }
        }
    }
    else if (m_Tag == GameObjectTag::Enemy)
    {
        for (const auto& gameObject : allGameObjects)
        {
            if (gameObject.get() == GetOwner()) continue;

            const auto otherHitbox{gameObject->GetComponent<dae::HitboxComponent>()};
            const auto otherPlayer{gameObject->GetComponent<dae::PlayerComponent>()};

            if (otherHitbox && otherPlayer && otherPlayer->GetTag() == GameObjectTag::Player)
            {
                if (ourHitbox->IsOverlapping(otherHitbox))
                {
                    otherPlayer->TakeDamage(1);
                    return;
                }
            }
        }
    }
}