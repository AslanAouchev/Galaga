#pragma once
#include "Component.h"
#include "GameObject.h"
#include <vector>
#include <functional>

namespace dae
{
    struct Rectangle
    {
        float x{};
        float y{};
        float width{};
        float height{};

        Rectangle() = default;
        Rectangle(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) 
        {
        }
    };

    class HitboxComponent final : public Component
    {
    public:
        void Update(const float deltaTime) override;
        void Render() const override {}

        HitboxComponent(GameObject* pOwner, float width, float height);

        HitboxComponent(GameObject* pOwner);

        virtual ~HitboxComponent() = default;
        HitboxComponent(const HitboxComponent& other) = delete;
        HitboxComponent(HitboxComponent&& other) = delete;
        HitboxComponent& operator=(const HitboxComponent& other) = delete;
        HitboxComponent& operator=(HitboxComponent&& other) = delete;

        bool IsOverlapping(const HitboxComponent* other) const;
        bool IsOverlapping(const Rectangle& rect) const;

        Rectangle GetHitbox() const;

        void SetSize(float width, float height);
        void SetOffset(float x, float y); 

        float GetWidth() const { return m_Width; }
        float GetHeight() const { return m_Height; }

    private:
        void UpdateSizeFromTexture();

        float m_Width{ 0.0f };
        float m_Height{ 0.0f };
        float m_OffsetX{ 0.0f };
        float m_OffsetY{ 0.0f };
        bool m_SizeCached{ false };
    };
}