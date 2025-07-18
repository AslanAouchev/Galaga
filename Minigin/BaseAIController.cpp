#include "BaseAIController.h"
#include "EnemyStates.h"
#include "GameObject.h"

BaseAIController::BaseAIController(dae::GameObject* owner)
    : m_Owner(owner)
{
    m_CurrentState = EnemyState::CreateFormationState();
}

BaseAIController::~BaseAIController() = default;

void BaseAIController::Update(float deltaTime)
{
    m_TargetUpdateTimer += deltaTime;
    if (m_TargetUpdateTimer >= m_TargetUpdateInterval)
    {
        UpdateTargetPlayer();
        m_TargetUpdateTimer = 0.0f;
    }

    auto newState{ m_CurrentState->Update(this, deltaTime) };
    if (newState)
    {
        SetState(std::move(newState));
    }
}

void BaseAIController::SetState(std::unique_ptr<EnemyState> newState)
{
    if (m_CurrentState)
    {
        m_CurrentState->Exit(this);
    }

    m_CurrentState = std::move(newState);

    if (m_CurrentState)
    {
        m_CurrentState->Enter(this);
    }
}

bool BaseAIController::IsNearFormation(float threshold) const
{
    return GetDistanceToPosition(m_FormationPosition) < threshold;
}

float BaseAIController::GetDistanceToPosition(const glm::vec3& target) const
{
    const auto currentPos{ m_Owner->GetTransform().GetPosition() };
    return glm::length(target - currentPos);
}

void BaseAIController::MoveTowards(const glm::vec3& target, float speed, float deltaTime)
{
    const auto currentPos{ m_Owner->GetTransform().GetPosition() };
    const auto direction{ glm::normalize(target - currentPos) };
    const auto newPos{ currentPos + direction * speed * deltaTime };
    m_Owner->SetPosition(newPos.x, newPos.y);
}

dae::GameObject* BaseAIController::GetClosestPlayer() const
{
    if (m_Players.empty()) return nullptr;

    dae::GameObject* closest{ nullptr };
    float minDistance{ std::numeric_limits<float>::max() };

    for (auto* const player : m_Players)
    {
        if (player)
        {
            const float distance{ GetDistanceToPlayer(player) };
            if (distance < minDistance)
            {
                minDistance = distance;
                closest = player;
            }
        }
    }

    return closest;
}

void BaseAIController::AddPlayer(dae::GameObject* player)
{
    if (player && std::find(m_Players.begin(), m_Players.end(), player) == m_Players.end())
    {
        m_Players.push_back(player);

        if (!m_TargetPlayer)
        {
            m_TargetPlayer = player;
        }
    }
}

void BaseAIController::RemovePlayer(dae::GameObject* player)
{
    const auto it{ std::find(m_Players.begin(), m_Players.end(), player) };
    if (it != m_Players.end())
    {
        m_Players.erase(it);

        if (m_TargetPlayer == player)
        {
            UpdateTargetPlayer();
        }
    }
}

void BaseAIController::UpdateTargetPlayer()
{
    m_Players.erase(
        std::remove(m_Players.begin(), m_Players.end(), nullptr),
        m_Players.end());

    if (m_Players.empty())
    {
        m_TargetPlayer = nullptr;
        return;
    }

    dae::GameObject* closest{ GetClosestPlayer() };

    if (!m_TargetPlayer || !closest)
    {
        m_TargetPlayer = closest;
        return;
    }

    const float currentTargetDistance{ GetDistanceToPlayer(m_TargetPlayer) };
    const float closestDistance{ GetDistanceToPlayer(closest) };

    if (closestDistance < currentTargetDistance * 0.7f)
    {
        m_TargetPlayer = closest;
    }
}

float BaseAIController::GetDistanceToPlayer(dae::GameObject* player) const
{
    if (!player) return std::numeric_limits<float>::max();

    const auto playerPos{ player->GetTransform().GetPosition() };
    return GetDistanceToPosition(playerPos);
}

void BaseAIController::OnNotify(const EventData& event)
{
    if (event.eventType == "ManagerPlayerHit")
    {
        SetState(m_CurrentState->CreateInDeadPlayerFormationState());
    }
    else if (event.eventType == "ResumeKilled")
    {
        SetState(m_CurrentState->CreateFormationState());
    }
    else if (event.eventType == "PauseUI")
    {
        m_Paused = true;
    }
	else if (event.eventType == "Resume")
	{
		m_Paused = false;
	}
    else if (event.eventType == "Reset")
    {
        m_Paused = false;
        m_TargetPlayer = nullptr;
        m_Players.clear();
        SetState(EnemyState::CreateFormationState());
        m_FormationPosition = { 0, 0, 0 };
        m_TargetUpdateTimer = 0.f;
    }
}

dae::GameObject* BaseAIController::GetClosestPlayerInRange(float range) const
{
    dae::GameObject* closest{ nullptr };
    float minDistance{ range };

    for (auto* const player : m_Players)
    {
        if (player)
        {
            const float distance{ GetDistanceToPlayer(player) };
            if (distance < minDistance)
            {
                minDistance = distance;
                closest = player;
            }
        }
    }

    return closest;
}
