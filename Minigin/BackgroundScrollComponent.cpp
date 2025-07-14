#include "BackgroundScrollComponent.h"
#include "GameObject.h"

namespace dae
{
    BackgroundScrollComponent::BackgroundScrollComponent(GameObject* owner, float scrollSpeed, float textureHeight, float startingPosition)
		: Component(owner), m_ScrollSpeed(scrollSpeed), m_TextureHeight(textureHeight), m_StartingPosition(startingPosition)
    {
        m_InitialY = GetOwner()->GetTransform().GetPosition().y;
    }

    void dae::BackgroundScrollComponent::Render() const
    {
    }

    void BackgroundScrollComponent::OnNotify(const EventData& event)
    {
        if (event.eventType == "PlayerHit")
        {
            m_KilledPaused = true;
        }
        else if (event.eventType == "EnemyKilled")
        {

        }
        else if (event.eventType == "PauseButton")
        {

        }
    }

    void BackgroundScrollComponent::Update(float deltaTime)
    {
        if (m_Paused || m_KilledPaused) return;

        m_CurrentOffset += m_ScrollSpeed * deltaTime;
        float newY{ m_InitialY + m_CurrentOffset };

        if (newY >= m_TextureHeight + m_StartingPosition)
        {
            m_CurrentOffset = m_StartingPosition;
            newY = m_InitialY;
        }

        auto currentPos = GetOwner()->GetTransform().GetPosition();
        GetOwner()->SetPosition(currentPos.x, newY);
    }
}