#pragma once
#include <memory>
#include "Transform.h"
#include <vector>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class GameObject final 
	{
	public:
		void Update(const float deltaTime);
		void Render() const;

		GameObject() = default;
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		template <typename T, typename... Args>
		void AddComponent(Args&&... args)
		{
			m_Components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
		}

		void RemoveComponent(Component* components);

		bool AreComponentsAdded() { return m_Components.empty(); };

		template <typename T>
		bool HasComponent()
		{
			for (const auto& component : m_Components) {
				if (dynamic_cast<T*>(component.get())) {
					return true;
				}
			}
			return false;
		};

		template <typename T>
		T* GetComponent()
		{
			for (const auto& component : m_Components) {
				if (auto casted = dynamic_cast<T*>(component.get())) {
					return casted;
				}
			}
			return nullptr;
		};

		void SetPosition(float x, float y);
		Transform GetTransform() const { return m_transform; };

		GameObject* GetParent() const { return m_Parent; }
		void SetParent(GameObject* pParent, bool keepWorldPosition);
		size_t GetChildCount() const { return m_Children.size(); }
		GameObject* GetChildAt(unsigned int index) const { return m_Children.at(index); }
		const std::vector<GameObject*>& GetChildren() const { return m_Children; }

		void MarkForDestruction() { m_markedForDestruction = true; }
		bool IsMarkedForDestruction() const { return m_markedForDestruction; }

		void AddObserver(Observer* observer)
		{
			if (observer && std::find(m_Observers.begin(), m_Observers.end(), observer) == m_Observers.end())
			{
				m_Observers.push_back(observer);
			}
		}

		void RemoveObserver(Observer* observer)
		{
			auto it{ std::find(m_Observers.begin(), m_Observers.end(), observer) };
			if (it != m_Observers.end())
			{
				m_Observers.erase(it);
			}
		}

		void NotifyObservers(const EventData& event)
		{
			auto observersCopy{ m_Observers };
			for (auto* observer : observersCopy)
			{
				if (observer)
				{
					observer->OnNotify(event);
				}
			}
		}

		void TriggerEvent(const std::string& eventType)
		{
			EventData event(eventType, this);
			NotifyObservers(event);
		}

	private:

		void AddChild(GameObject* pChild);
		void RemoveChild(GameObject* pChildToRemove);
		bool IsChild(GameObject* pChildToCompare);

		glm::vec3 GetWorldPosition();
		void UpdateWorldPosition();
		void SetPositionDirty();
		void SetLocalPosition(const glm::vec3& pos);

		Transform m_transform{};
		std::vector<std::unique_ptr<Component>> m_Components{};
		GameObject* m_Parent{nullptr};
		std::vector<GameObject*> m_Children{};
		std::vector<Observer*> m_Observers;
		bool m_positionIsDirty{true};
		glm::vec3 m_WorldPosition{};
		glm::vec3 m_LocalPosition{};
		bool m_markedForDestruction{ false };
	};
}
