#include "EnemyStates.h"
#include "BaseAIController.h"
#include <iostream>

std::unique_ptr<EnemyState> EnemyState::CreateFormationState()
{
    return std::make_unique<FormationState>();
}

std::unique_ptr<EnemyState> EnemyState::CreateInDeadPlayerFormationState()
{
    return std::make_unique<InDeadPlayerFormationState>();
}

std::unique_ptr<EnemyState> EnemyState::CreateInFormationState()
{
    return std::make_unique<InFormationState>();
}

std::unique_ptr<EnemyState> EnemyState::CreateDivingState()
{
    return std::make_unique<DivingState>();
}

std::unique_ptr<EnemyState> FormationState::Update(BaseAIController* controller, float deltaTime)
{
    controller->MoveTowards(
        controller->GetFormationPosition(),
        controller->GetSpeed(),
        deltaTime
    );

    if (controller->IsNearFormation(10.0f))
    {
        return EnemyState::CreateInFormationState();
    }

    return nullptr;
}

void FormationState::Enter(BaseAIController*)
{
    std::cout << "entering formation\n";
}

std::unique_ptr<EnemyState> InFormationState::Update(BaseAIController* controller, float deltaTime)
{
    controller->OnUpdateFormationBehavior(deltaTime);

    m_DiveTimer += deltaTime;

    if (!controller->GetPlayers().empty())
    {
        if (controller->OnShouldDive())
        {
            if (m_DiveTimer >= 5.0f)
            {
                return EnemyState::CreateDivingState();
            }
        }
    }

    return nullptr;
}

void InFormationState::Enter(BaseAIController*)
{
    std::cout << "in formation\n";
    m_DiveTimer = 0.0f;
}

std::unique_ptr<EnemyState> DivingState::Update(BaseAIController* controller, float deltaTime)
{
    if (!m_PathGenerated)
    {
        controller->OnGenerateDivePath(m_DivePath);
        m_PathGenerated = true;
        m_CurrentPathPoint = 0;
    }

    m_ShootTimer += deltaTime;
    if (m_ShootTimer >= m_ShootInterval && controller->GetTargetPlayer())
    {
        controller->Shoot();
        m_ShootTimer = 0.0f;
    }

    if (m_CurrentPathPoint < m_DivePath.size())
    {
        const glm::vec3 target{ m_DivePath[m_CurrentPathPoint] };
        const float distanceToTarget{ controller->GetDistanceToPosition(target) };
        controller->MoveTowards(target, controller->GetSpeed() * 1.5f, deltaTime);
        if (distanceToTarget < 15.0f)
        {
            m_CurrentPathPoint++;
        }
    }

    if (m_CurrentPathPoint >= m_DivePath.size())
    {
        return EnemyState::CreateFormationState();
    }

    const auto currentPos{ controller->GetOwnerAI()->GetTransform().GetPosition() };
    if (currentPos.y > 480.f || currentPos.x < -50.0f || currentPos.x > 680.0f)
    {
        return EnemyState::CreateFormationState();
    }

    return nullptr;
}

void DivingState::Enter(BaseAIController*)
{
    std::cout << "starting dive attack\n";
    m_DivePath.clear();
    m_CurrentPathPoint = 0;
    m_PathGenerated = false;
}

std::unique_ptr<EnemyState> InDeadPlayerFormationState::Update(BaseAIController* controller, float deltaTime)
{
	if (controller->IsNearFormation(10.0f))
	{
        controller->OnUpdateFormationBehavior(deltaTime);
    }
    else
    {
        controller->MoveTowards(
            controller->GetFormationPosition(),
            controller->GetSpeed(),
            deltaTime
        );
    }

    return nullptr;
}

void InDeadPlayerFormationState::Enter(BaseAIController*)
{
    std::cout << "starting in dead player formation state\n";
}
