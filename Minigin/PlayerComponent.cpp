#include "PlayerComponent.h"
#include "../Galaga/BulletComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "HitboxComponent.h"

dae::PlayerComponent::PlayerComponent(GameObject* pOwner, const std::string& textureFileName, const std::string& bulletTextureFileName, int health, float bulletSpeed) :
	Component(pOwner), m_MaxHealth{ health }, m_BulletString{ bulletTextureFileName }, m_BulletSpeed{ bulletSpeed }
{
    m_pTexture = std::make_unique<TextureComponent>(textureFileName, pOwner);
	GetOwner()->AddComponent<dae::HitboxComponent>(GetOwner());
	m_Health = m_MaxHealth;
}

void dae::PlayerComponent::Update(const float deltaTime)
{
    UpdateKilledPause(deltaTime);

    if (m_Paused || m_KilledPaused) 
    {
        return;
    }

    if (m_pTexture)
    {
        m_pTexture->Update(deltaTime);
    }

    if (m_FireCooldown > 0.0f)
    {
        m_FireCooldown -= deltaTime;
    }

    if (m_IsDead && m_Tag != GameObjectTag::Player)
    {
        GetOwner()->SetActive(false);
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
    if (m_Paused || m_KilledPaused)
    {
        return;
    }

    m_Health -= amount;
    ServiceLocator::getSoundSystem().play(5, 0.8f);

    if (m_Tag == GameObjectTag::Player)
    {
        GetOwner()->TriggerEvent("PlayerHit");
    }

    if (m_Health <= 0)
    {
        if (m_Tag == GameObjectTag::Bee)
        {
            GetOwner()->TriggerEvent("EnemyKilled");
        }
		else if (m_Tag == GameObjectTag::Butterfly)
		{
			GetOwner()->TriggerEvent("EnemyKilled");
		}
		else if (m_Tag == GameObjectTag::Boss)
		{
			GetOwner()->TriggerEvent("EnemyKilled");
		}
		else if (m_Tag == GameObjectTag::EnemyPlayer)
		{
			GetOwner()->TriggerEvent("EnemyKilled");
		}
        m_IsDead = true;
    }
}

bool dae::PlayerComponent::Fire()
{
    if (m_Paused || m_KilledPaused)
    {
        return false;
    }

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

void dae::PlayerComponent::OnNotify(const EventData& event)
{
	if (event.eventType == "PlayerHit")
	{
        m_KilledPaused = true;
        m_KilledPauseTimer = 5.f;
	}
	else if (event.eventType == "ResumeKillled")
	{
		m_KilledPaused = false;
		m_KilledPauseTimer = 0.0f;
	}
	else if (event.eventType == "Pause")
	{
		m_Paused = true;
	}
    else if (event.eventType == "Resume")
    {
        m_Paused = false;
    }
    else if (event.eventType == "Reset")
    {
        m_Health = m_MaxHealth;
        m_IsDead = false;
        m_Paused = false;
        m_FireCooldown = 0.0f;
        GetOwner()->SetActive(false);
    }
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
                if (otherPlayer->GetTag() != GameObjectTag::Player)
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
    else if (m_Tag != GameObjectTag::Player)
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

void dae::PlayerComponent::UpdateKilledPause(float deltaTime)
{
    if (m_KilledPaused)
    {
        m_KilledPauseTimer -= deltaTime;
        if (m_KilledPauseTimer <= 0.0f)
        {
            m_KilledPaused = false;
            m_KilledPauseTimer = 0.0f;
            GetOwner()->TriggerEvent("ResumeKilled");
        }
    }
}
