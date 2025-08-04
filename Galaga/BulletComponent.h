#pragma once
#include <memory>
#include "GameObject.h"
#include "Component.h"
#include "TextureComponent.h"

namespace dae
{
    enum class BulletTag;

	class BulletComponent final : public Component, public Observer
    {
    public:
        void Update(const float deltaTime) override;
        void Render() const override;
        BulletComponent(GameObject* pOwner, const std::string& textureFileName, BulletTag tag, float speed);
        virtual ~BulletComponent() = default;
        BulletComponent(const BulletComponent& other) = delete;
        BulletComponent(BulletComponent&& other) = delete;
        BulletComponent& operator=(const BulletComponent& other) = delete;
        BulletComponent& operator=(BulletComponent&& other) = delete;

        bool IsDestroyed() const { return m_IsDestroyed; }
        void SetDirection(float x, float y) { m_DirectionX = x; m_DirectionY = y; }

        BulletTag GetTag() const { return m_BulletTag; }
        void SetTag(BulletTag tag) { m_BulletTag = tag; }

    private:
        void CheckCollisions();
        void Destroy() { m_IsDestroyed = true; }

        virtual void OnNotify(const EventData& event) override;

        float m_Speed{};
        float m_DirectionX{ 0.0f };
        float m_DirectionY{ -1.0f };
        float m_StrafeDirection{};
        bool m_IsDestroyed{ false };
        bool m_IsPaused{};
        BulletTag m_BulletTag;

        std::unique_ptr<dae::TextureComponent> m_pTexture{ nullptr };
    };
}