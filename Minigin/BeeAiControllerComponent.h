#pragma once

#include "Component.h"
#include "GameObject.h"
#include "BaseAIController.h"

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
    void Render() const override {}

private:
    float m_FormationWobble{};
    float m_ExistenceTimer{};
    bool m_HasFoundPlayers{};

    void OnUpdateFormationBehavior(float deltaTime) override;
    void OnGenerateDivePath(std::vector<glm::vec3>& path) override;
    bool OnShouldDive() override;
	void Shoot() override;
    void UpdatePlayersFromScene();
};

