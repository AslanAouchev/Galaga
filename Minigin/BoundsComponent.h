#pragma once
#include "Component.h"
#include "GameObject.h"

namespace dae
{
    class BoundsComponent final : public Component
    {
    public:
        void Update(const float deltaTime) override;
        void Render() const override {}

        BoundsComponent(GameObject* pOwner, float windowWidth, float windowHeight);
        virtual ~BoundsComponent() = default;

        BoundsComponent(const BoundsComponent& other) = delete;
        BoundsComponent(BoundsComponent&& other) = delete;
        BoundsComponent& operator=(const BoundsComponent& other) = delete;
        BoundsComponent& operator=(BoundsComponent&& other) = delete;

    private:
        float m_WindowWidth{};
        float m_WindowHeight{};

        float m_ObjectWidth{ 0.0f };
        float m_ObjectHeight{ 0.0f };
        bool m_SizeDirtied{ false };
    };
}