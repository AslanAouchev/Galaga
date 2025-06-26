#pragma once
#include "Component.h"

namespace dae
{
    class BackgroundScrollComponent : public Component
    {
    public:
        BackgroundScrollComponent(GameObject* owner, float scrollSpeed, float textureHeight, float startingPosition);
        void Update(float deltaTime) override;
        void Render() const override;
    private:
        float m_StartingPosition{};
        float m_ScrollSpeed{};
        float m_CurrentOffset{};
        float m_TextureHeight{};
        float m_InitialY{};
    };
}