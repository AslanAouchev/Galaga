#pragma once
#include <memory>
#include "GameObject.h"
#include "Component.h"

namespace dae
{
	class MovementComponent final : public Component
	{
	public:
		void Update(const float deltaTime) override;
		void Render() const override;

		MovementComponent(GameObject* pOwner, float radius = 10.f, float angle = 0.f, float speed = 10.f);
		virtual ~MovementComponent() = default;
		MovementComponent(const MovementComponent& other) = delete;
		MovementComponent(MovementComponent&& other) = delete;
		MovementComponent& operator=(const MovementComponent& other) = delete;
		MovementComponent& operator=(MovementComponent&& other) = delete;

	private:
		glm::vec3 m_InitialPosition{};
		glm::vec3 m_Position{};
		float m_Angle{};
		float m_Radius{};
		float m_Speed{};
	};
}
