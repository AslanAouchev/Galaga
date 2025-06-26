#include "BoundsComponent.h"
#include "TextureComponent.h"
#include "PlayerComponent.h"
#include <iostream>

dae::BoundsComponent::BoundsComponent(GameObject* pOwner, float windowWidth, float windowHeight)
    : Component(pOwner), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
{
}

void dae::BoundsComponent::Update(const float)
{
    if (!m_SizeDirtied)
    {
        if (auto textureComp = GetOwner()->GetComponent<dae::TextureComponent>())
        {
            const auto textureSize{ textureComp->GetTextureSize() };
            m_ObjectWidth = static_cast<float>(textureSize.x);
            m_ObjectHeight = static_cast<float>(textureSize.y);
            m_SizeDirtied = true;
        }
        else if (auto playerComp = GetOwner()->GetComponent<dae::PlayerComponent>())
        {
            const auto textureSize{ playerComp->GetTextureSize() };
            m_ObjectWidth = static_cast<float>(textureSize.x);
            m_ObjectHeight = static_cast<float>(textureSize.y);
            m_SizeDirtied = true;
        }
    }

    const auto currentPos{ GetOwner()->GetTransform().GetPosition() };
    float newX{static_cast<float>(currentPos.x)};
    float newY{ static_cast<float>(currentPos.y) };

    if (newX < 0) newX = 0;
    if (newX > m_WindowWidth - m_ObjectWidth) newX = m_WindowWidth - m_ObjectWidth;
    if (newY < 0) newY = 0;
    if (newY > m_WindowHeight - m_ObjectHeight) newY = m_WindowHeight - m_ObjectHeight;

    GetOwner()->SetPosition(newX, newY);
}