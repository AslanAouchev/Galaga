#pragma once
#include <memory>
#include "GameObject.h"
#include "Component.h"
#include "TextureComponent.h"

namespace dae
{
    enum class GameObjectTag;

    class BulletComponent final : public Component
    {
    public:
        void Update(const float deltaTime) override;
        void Render() const override;
        BulletComponent(GameObject* pOwner, const std::string& textureFileName, GameObjectTag tag, float speed);
        virtual ~BulletComponent() = default;
        BulletComponent(const BulletComponent& other) = delete;
        BulletComponent(BulletComponent&& other) = delete;
        BulletComponent& operator=(const BulletComponent& other) = delete;
        BulletComponent& operator=(BulletComponent&& other) = delete;

        bool IsOffScreen() const { return m_IsOffScreen; }
        bool IsDestroyed() const { return m_IsDestroyed; }
        void SetDirection(float x, float y) { m_DirectionX = x; m_DirectionY = y; }

        GameObjectTag GetTag() const { return m_BulletTag; }
        void SetTag(GameObjectTag tag) { m_BulletTag = tag; }

    private:
        void CheckCollisions();
        void Destroy() { m_IsDestroyed = true; }

        float m_Speed{};
        float m_DirectionX{ 0.0f };
        float m_DirectionY{ -1.0f };
        bool m_IsOffScreen{ false };
        bool m_IsDestroyed{ false };
        GameObjectTag m_BulletTag;

        std::unique_ptr<dae::TextureComponent> m_pTexture{ nullptr };
    };
}