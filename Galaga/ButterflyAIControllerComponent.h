#pragma once
#include "Component.h"
#include "GameObject.h"
#include "BaseAIController.h"

class ButterflyAIControllerComponent : public dae::Component, public BaseAIController
{
public:
    ButterflyAIControllerComponent(dae::GameObject* owner);
    virtual ~ButterflyAIControllerComponent() = default;

    ButterflyAIControllerComponent(const ButterflyAIControllerComponent& other) = delete;
    ButterflyAIControllerComponent(ButterflyAIControllerComponent&& other) = delete;
    ButterflyAIControllerComponent& operator=(const ButterflyAIControllerComponent& other) = delete;
    ButterflyAIControllerComponent& operator=(ButterflyAIControllerComponent&& other) = delete;

    void Update(const float deltaTime) override;
    void Render() const override;

    void OnUpdateFormationBehavior(float deltaTime) override;
    void OnGenerateDivePath(std::vector<glm::vec3>& path) override;
    void Shoot() override;

private:
    float m_EvasiveTimer{};
    float m_EvasiveInterval{ 0.3f };
    int m_EvasiveDirection{ 1 };
    float m_EvasiveIntensity{ 80.0f };

    float m_FormationHover{};

    bool m_IsInEvasiveMode{};
    float m_CurrentDiveSpeed{ 150.0f };
};