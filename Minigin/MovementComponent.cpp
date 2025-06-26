#include "MovementComponent.h"

void dae::MovementComponent::Update(const float deltaTime)
{
	m_Angle += m_Speed * deltaTime;
	m_Position.x = m_InitialPosition.x + m_Radius * cos(m_Angle);
	m_Position.y = m_InitialPosition.y + m_Radius * sin(m_Angle);
	GetOwner()->SetPosition(m_Position.x, m_Position.y);
}

void dae::MovementComponent::Render() const
{
}

dae::MovementComponent::MovementComponent(GameObject* pOwner, float radius, float angle, float speed):
	Component(pOwner), m_Radius(radius), m_Angle(angle), m_Speed(speed)
{
	m_InitialPosition = GetOwner()->GetTransform().GetPosition();
}
