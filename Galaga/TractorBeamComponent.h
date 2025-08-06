#pragma once
#include "Component.h"
#include "GameObject.h"
#include <functional>
#include "TextureComponent.h"

class BossAIControllerComponent;

class TractorBeamComponent : public dae::Component
{
public:
    TractorBeamComponent(dae::GameObject * owner, BossAIControllerComponent* bossController);
    virtual ~TractorBeamComponent() = default;

    TractorBeamComponent(const TractorBeamComponent& other) = delete;
    TractorBeamComponent(TractorBeamComponent&& other) = delete;
    TractorBeamComponent& operator=(const TractorBeamComponent& other) = delete;
    TractorBeamComponent& operator=(TractorBeamComponent&& other) = delete;

    void Update(const float deltaTime) override;
    void Render() const override;

    bool IsPositionInBeam(const glm::vec3& position) const;

private:
    void OnPlayerHit();
    void CheckPlayerCollisions();

    std::unique_ptr<dae::TextureComponent> m_TextureComponent;
    BossAIControllerComponent* m_BossController{ nullptr };

    bool m_HasHitPlayer{};

    float m_BeamWidth{ 80.0f };
    float m_BeamHeight{ 400.0f };
};