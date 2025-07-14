#pragma once
#include <memory>
#include <glm.hpp>
#include <vector>

class BaseAIController;

class EnemyState
{
public:
    static std::unique_ptr<EnemyState> CreateFormationState();
    static std::unique_ptr<EnemyState> CreateInFormationState();
    static std::unique_ptr<EnemyState> CreateDivingState();
    static std::unique_ptr<EnemyState> CreateInDeadPlayerFormationState();
    virtual ~EnemyState() = default;
    virtual std::unique_ptr<EnemyState> Update(BaseAIController* controller, float deltaTime) = 0;
    virtual void Enter(BaseAIController* ) {  }
    virtual void Exit(BaseAIController* ) {  }

protected:
    EnemyState() = default;
};

class FormationState : public EnemyState
{
public:
    std::unique_ptr<EnemyState> Update(BaseAIController* controller, float deltaTime) override;
    void Enter(BaseAIController* controller) override;
};

class InFormationState : public EnemyState
{
public:
    std::unique_ptr<EnemyState> Update(BaseAIController* controller, float deltaTime) override;
    void Enter(BaseAIController* controller) override;

private:
    float m_DiveTimer{};
};

class DivingState : public EnemyState
{
public:
    std::unique_ptr<EnemyState> Update(BaseAIController* controller, float deltaTime) override;
    void Enter(BaseAIController* controller) override;

private:
    std::vector<glm::vec3> m_DivePath;
    size_t m_CurrentPathPoint{};
    bool m_PathGenerated{};
    float m_ShootTimer{};
    float m_ShootInterval{ 0.5f };
};

class InDeadPlayerFormationState : public EnemyState
{
public:
    std::unique_ptr<EnemyState> Update(BaseAIController* controller, float deltaTime) override;
    void Enter(BaseAIController* controller) override;
};