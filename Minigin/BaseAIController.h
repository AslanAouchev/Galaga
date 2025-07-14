#pragma once
#include <memory>
#include <glm.hpp>
#include <vector>
#include <functional>
#include "GameObject.h"
#include "Observer.h"

class EnemyState;

class BaseAIController : public Observer
{
public:
    BaseAIController(dae::GameObject* owner);
    ~BaseAIController();

    void Update(float deltaTime);
    void SetState(std::unique_ptr<EnemyState> newState);

	dae::GameObject* GetOwnerAI() const { return m_Owner; }
    const std::vector<dae::GameObject*>& GetPlayers() const { return m_Players; }
    dae::GameObject* GetClosestPlayer() const;
    dae::GameObject* GetTargetPlayer() const { return m_TargetPlayer; }
    const glm::vec3& GetFormationPosition() const { return m_FormationPosition; }
    float GetSpeed() const { return m_Speed; }
    bool IsNearFormation(float threshold = 5.0f) const;
    float GetDistanceToPosition(const glm::vec3& target) const;
    void MoveTowards(const glm::vec3& target, float speed, float deltaTime);

    void SetPlayers(const std::vector<dae::GameObject*>& players) { m_Players = players; }
    void AddPlayer(dae::GameObject* player);
    void RemovePlayer(dae::GameObject* player);
    void SetTargetPlayer(dae::GameObject* player) { m_TargetPlayer = player; }
    void SetFormationPosition(const glm::vec3& pos) { m_FormationPosition = pos; }
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetDiveCooldown(float cooldown) { m_DiveCooldown = cooldown; }

    void UpdateTargetPlayer();
    float GetDistanceToPlayer(dae::GameObject* player) const;
    void OnNotify(const EventData& event) override;
    dae::GameObject* GetClosestPlayerInRange(float range) const;

    virtual void OnUpdateFormationBehavior(float) = 0;
    virtual void OnGenerateDivePath(std::vector<glm::vec3>&) = 0;
    virtual bool OnShouldDive() = 0;
    virtual void Shoot() = 0;

protected:
    bool m_Paused{ false };

private:
    std::unique_ptr<EnemyState> m_CurrentState{nullptr};
    dae::GameObject* m_Owner{nullptr};
    std::vector<dae::GameObject*> m_Players;
    dae::GameObject* m_TargetPlayer{nullptr};
    glm::vec3 m_FormationPosition{ 0, 0, 0 };
    float m_Speed{60.f};
    float m_DiveCooldown{ 5.0f };
    float m_TargetUpdateTimer{};
    float m_TargetUpdateInterval{ 1.f };
};