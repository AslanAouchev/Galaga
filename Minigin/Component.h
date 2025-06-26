#pragma once

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		virtual void Update(const float deltaTime) = 0;
		virtual void Render() const = 0;

		virtual ~Component();
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		GameObject* GetOwner() const { return m_pOwner; }

	protected:
		explicit Component(GameObject* pOwner) : m_pOwner(pOwner) {}
	private:

		GameObject* m_pOwner{ nullptr };
	};
}
