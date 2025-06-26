#pragma once

#include "Component.h"
#include "GameObject.h"
#include "BaseAIController.h"

class BeeAiControllerComponent : public dae::Component
{
public:
    BeeAiControllerComponent(dae::GameObject* owner);
    virtual ~BeeAiControllerComponent() = default;

    void Update(const float deltaTime) override;
    void Render() const override {}

    void SetPlayers(const std::vector<dae::GameObject*>& players) { m_AICore->SetPlayers(players); }
    void AddPlayer(dae::GameObject* player) { m_AICore->AddPlayer(player); }
    void RemovePlayer(dae::GameObject* player) { m_AICore->RemovePlayer(player); }
    void SetFormationPosition(const glm::vec3& pos) { m_AICore->SetFormationPosition(pos); }

private:
    std::unique_ptr<BaseAIController> m_AICore;
    float m_FormationWobble{};
    bool m_HasFoundPlayers{};
    float m_ExistenceTimer{};

    void UpdateFormationBehavior(float deltaTime);
    void GenerateDivePath(std::vector<glm::vec3>& path);
    bool ShouldDive();
	void Shoot();
    void UpdatePlayersFromScene();
};

