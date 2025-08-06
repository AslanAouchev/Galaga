#include "EnemyStates.h"
#include "BaseAIController.h"
#include <iostream>
#include <PlayerComponent.h>
#include <BeeAiControllerComponent.h>
#include "../Galaga/BossAIControllerComponent.h"
#include "../Galaga/ButterflyAIControllerComponent.h"

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

void FormationState::Enter(BaseAIController* controller)
{
    auto playerComp = controller->GetOwnerAI()->GetComponent<dae::PlayerComponent>();

    if(playerComp)
    {
        if (controller->GetOwnerAI()->GetComponent<BeeAiControllerComponent>())
        {
            playerComp->SetScore(50);
        }
        else if (controller->GetOwnerAI()->GetComponent<BossAIControllerComponent>())
        {
            playerComp->SetScore(150);
        }
        else if (controller->GetOwnerAI()->GetComponent<ButterflyAIControllerComponent>())
        {
            playerComp->SetScore(80);
        }
    }

    controller->SetAttack(false);
}

std::unique_ptr<EnemyState> InFormationState::Update(BaseAIController* controller, float deltaTime)
{
    controller->OnUpdateFormationBehavior(deltaTime);

    if (!controller->GetPlayers().empty())
    {
        if (controller->OnShouldDive())
        {
            return EnemyState::CreateDivingState();
        }
    }

    return nullptr;
}

void InFormationState::Enter(BaseAIController* controller)
{
    auto playerComp = controller->GetOwnerAI()->GetComponent<dae::PlayerComponent>();

    if (playerComp)
    {
        if (controller->GetOwnerAI()->GetComponent<BeeAiControllerComponent>())
        {
            playerComp->SetScore(50);
        }
        else if (controller->GetOwnerAI()->GetComponent<BossAIControllerComponent>())
        {
            playerComp->SetScore(150);
        }
        else if (controller->GetOwnerAI()->GetComponent<ButterflyAIControllerComponent>())
        {
            playerComp->SetScore(80);
        }
    }

    controller->SetAttack(false);
}

std::unique_ptr<EnemyState> DivingState::Update(BaseAIController* controller, float deltaTime)
{
    if (!m_PathGenerated)
    {
        controller->OnGenerateDivePath(m_DivePath);
        m_PathGenerated = true;
        m_CurrentPathPoint = 0;
    }

    bool isTractorBeamActive{};
    if (m_IsBoss)
    {
        isTractorBeamActive = m_BossController->IsTractorBeamActive();
    }

    m_ShootTimer += deltaTime;
    if (m_ShootTimer >= m_ShootInterval && controller->GetTargetPlayer())
    {
        if (m_IsBoss)
        {
            const auto currentPos{ controller->GetOwnerAI()->GetTransform().GetPosition() };

            if (isTractorBeamActive && !m_TractorBeamUsed &&
                currentPos.y >= 330.0f && currentPos.y <= 350.0f)
            {
                m_BossController->ShootTractorBeam();
                m_TractorBeamUsed = true;
                m_ShootTimer = 0.0f;
            }
            if (!isTractorBeamActive)
            {
                controller->Shoot();
                m_ShootTimer = 0.0f;
            }
        }
        else
        {
            controller->Shoot();
            m_ShootTimer = 0.0f;
        }
    }

    if (m_CurrentPathPoint < m_DivePath.size())
    {
        const glm::vec3 target{ m_DivePath[m_CurrentPathPoint] };
        const float distanceToTarget{ controller->GetDistanceToPosition(target) };

        bool shouldStop{};
        if (m_IsBoss && isTractorBeamActive)
        {
            const auto currentPos{ controller->GetOwnerAI()->GetTransform().GetPosition() };
            if (currentPos.y >= 330.0f && currentPos.y <= 350.0f)
            {
                shouldStop = true;
            }
        }

        if (!shouldStop)
        {
            float baseSpeed{ controller->GetSpeed() };
            float speedMultiplier{ m_IsBoss ? 1.0f : 1.5f };

            controller->MoveTowards(target, baseSpeed * speedMultiplier, deltaTime);

            if (distanceToTarget < 15.0f)
            {
                m_CurrentPathPoint++;
            }
        }
    }

    if (m_CurrentPathPoint >= m_DivePath.size())
    {
        if (m_IsBoss && isTractorBeamActive)
        {
            return nullptr;
        }

        return EnemyState::CreateFormationState();
    }

    const auto currentPos{ controller->GetOwnerAI()->GetTransform().GetPosition() };
    if (currentPos.y > 480.f || currentPos.x < -50.0f || currentPos.x > 680.0f)
    {
        if(!m_IsBee)
        {
            return EnemyState::CreateFormationState();
        }
        else
        {
            if (m_BeeController->CanGoToFormation())
            {
                return EnemyState::CreateFormationState();
            }
        }
    }

    return nullptr;
}

void DivingState::Enter(BaseAIController* controller)
{
    auto playerComp = controller->GetOwnerAI()->GetComponent<dae::PlayerComponent>();

    m_BossController = dynamic_cast<BossAIControllerComponent*>(controller);
    m_IsBoss = (m_BossController != nullptr);

    m_BeeController = dynamic_cast<BeeAiControllerComponent*>(controller);
    m_IsBee = (m_BeeController != nullptr);

    if (playerComp)
    {
        if (m_IsBoss)
        {
            playerComp->SetScore(400);
			m_ShootInterval = 1.0f;
        }
        else if (m_IsBee)
        {
            playerComp->SetScore(100);
            m_ShootInterval = 1.25f;
        }
        else if (controller->GetOwnerAI()->GetComponent<ButterflyAIControllerComponent>())
        {
            playerComp->SetScore(160);
            m_ShootInterval = 0.75f;
        }
    }

    m_DivePath.clear();
    m_CurrentPathPoint = 0;
    m_PathGenerated = false;
    m_TractorBeamUsed = false;
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

void InDeadPlayerFormationState::Enter(BaseAIController* controller)
{
    auto playerComp = controller->GetOwnerAI()->GetComponent<dae::PlayerComponent>();

    if (playerComp)
    {
        if (controller->GetOwnerAI()->GetComponent<BeeAiControllerComponent>())
        {
            playerComp->SetScore(50);
        }
        else if (controller->GetOwnerAI()->GetComponent<BossAIControllerComponent>())
        {
            playerComp->SetScore(150);
        }
        else if (controller->GetOwnerAI()->GetComponent<ButterflyAIControllerComponent>())
        {
            playerComp->SetScore(80);
        }
    }
}
