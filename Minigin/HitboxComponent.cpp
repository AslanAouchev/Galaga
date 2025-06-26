#include "HitboxComponent.h"
#include "TextureComponent.h"
#include "PlayerComponent.h"

dae::HitboxComponent::HitboxComponent(GameObject* pOwner, float width, float height)
    : Component(pOwner), m_Width(width), m_Height(height), m_SizeCached(true)
{
}

dae::HitboxComponent::HitboxComponent(GameObject* pOwner)
    : Component(pOwner)
{
}

void dae::HitboxComponent::Update(const float)
{
    if (!m_SizeCached)
    {
        UpdateSizeFromTexture();
    }
}

void dae::HitboxComponent::UpdateSizeFromTexture()
{
    if (auto textureComp{ GetOwner()->GetComponent<dae::TextureComponent>() })
    {
        const auto size{ textureComp->GetTextureSize() };
        m_Width = static_cast<float>(size.x);
        m_Height = static_cast<float>(size.y);
        m_SizeCached = true;
    }
    else if (auto playerComp{ GetOwner()->GetComponent<dae::PlayerComponent>() })
    {
        const auto size{ playerComp->GetTextureSize() };
        m_Width = static_cast<float>(size.x);
        m_Height = static_cast<float>(size.y);
        m_SizeCached = true;
    }
}

bool dae::HitboxComponent::IsOverlapping(const HitboxComponent* other) const
{
    if (!other) return false;

    const Rectangle thisHitbox{ GetHitbox() };
    const Rectangle otherHitbox{ other->GetHitbox() };

    return IsOverlapping(otherHitbox);
}

bool dae::HitboxComponent::IsOverlapping(const Rectangle& rect) const
{
    const Rectangle thisHitbox{ GetHitbox() };

    return (thisHitbox.x < rect.x + rect.width && thisHitbox.x + thisHitbox.width > rect.x && thisHitbox.y < rect.y + rect.height && thisHitbox.y + thisHitbox.height > rect.y);
}

dae::Rectangle dae::HitboxComponent::GetHitbox() const
{
    const auto pos{ GetOwner()->GetTransform().GetPosition() };
    return Rectangle(pos.x + m_OffsetX,pos.y + m_OffsetY,m_Width,m_Height);
}

void dae::HitboxComponent::SetSize(float width, float height)
{
    m_Width = width;
    m_Height = height;
    m_SizeCached = true;
}

void dae::HitboxComponent::SetOffset(float x, float y)
{
    m_OffsetX = x;
    m_OffsetY = y;
}