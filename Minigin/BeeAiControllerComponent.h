#pragma once

#include "Component.h"
#include "GameObject.h"
#include "BaseAIController.h"
#include "PlayerComponent.h"

class BeeAiControllerComponent : public dae::Component, public BaseAIController
{
public:
    BeeAiControllerComponent(dae::GameObject* owner);
    ~BeeAiControllerComponent();

    BeeAiControllerComponent(const BeeAiControllerComponent& other) = delete;
    BeeAiControllerComponent(BeeAiControllerComponent&& other) = delete;
    BeeAiControllerComponent& operator=(const BeeAiControllerComponent& other) = delete;
    BeeAiControllerComponent& operator=(BeeAiControllerComponent&& other) = delete;

    void Update(const float deltaTime) override;
    void Render() const override {};

    bool CanGoToFormation() { return m_GoToFormation; }

private:
    float m_FormationWobble{};
    bool m_GoToFormation{};
    const float m_MaxDiveTime{5.f};
    float m_DivingTimer{};

    void OnUpdateFormationBehavior(float deltaTime) override;
    void OnGenerateDivePath(std::vector<glm::vec3>& path) override;
	void Shoot() override;
};

