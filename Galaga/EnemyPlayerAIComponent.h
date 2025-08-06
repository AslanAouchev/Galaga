#pragma once
#include "Component.h"
#include "GameObject.h"

class BossAIControllerComponent;

class EnemyPlayerAIComponent : public dae::Component, public Observer
{
public:
    EnemyPlayerAIComponent(dae::GameObject* owner, BossAIControllerComponent* boss);
    virtual ~EnemyPlayerAIComponent() = default;

    EnemyPlayerAIComponent(const EnemyPlayerAIComponent& other) = delete;
    EnemyPlayerAIComponent(EnemyPlayerAIComponent&& other) = delete;
    EnemyPlayerAIComponent& operator=(const EnemyPlayerAIComponent& other) = delete;
    EnemyPlayerAIComponent& operator=(EnemyPlayerAIComponent&& other) = delete;

    void Update(const float deltaTime) override;
    void Render() const override {};

    void Shoot();

private:
    virtual void OnNotify(const EventData& event) override;

    float m_ShootTimer{};
    float m_ShootInterval{ 2.f };
    bool m_DontShoot{ true };
    bool m_PlayerControlled{};
    BossAIControllerComponent* m_Boss{ nullptr };

};
