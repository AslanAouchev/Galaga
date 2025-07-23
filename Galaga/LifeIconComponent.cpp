#include "LifeIconComponent.h"
#include "GameObject.h"

LifeIconComponent::LifeIconComponent(dae::GameObject* owner, int iconIndex)
    : Component(owner), m_IconIndex(iconIndex)
{
}

LifeIconComponent::~LifeIconComponent()
{
}

void LifeIconComponent::OnNotify(const EventData& event)
{
    if (event.eventType == "SetLives")
    {
        int currentLives{ event.intValue };

        if (m_IconIndex < currentLives)
        {
            ShowIcon();
        }
        else
        {
            HideIcon();
        }
    }
}

void LifeIconComponent::ShowIcon()
{
    if (!m_IsVisible)
    {
        m_IsVisible = true;
        GetOwner()->SetPosition(static_cast<float>(20 + (m_IconIndex * 35)), 450.f);
    }
}

void LifeIconComponent::HideIcon()
{
    if (m_IsVisible)
    {
        m_IsVisible = false;
        GetOwner()->SetPosition(-10000, -10000);
    }
}