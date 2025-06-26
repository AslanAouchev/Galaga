#pragma once
#include "SceneManager.h"
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(GameObject* object);
		void RemoveAll();

		void Update(const float& deltaTime);
		void Render() const;

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::string& GetName() const { return m_name; }
		std::vector<std::unique_ptr<GameObject>>& GetAllGameObjects() { return m_objects; }

	private: 
		explicit Scene(const std::string& name);

		std::string m_name;
		std::vector<std::unique_ptr<GameObject>> m_objects{};

		float m_deltaTime{};
		bool m_shouldRemoveAll{ false };

		static unsigned int m_idCounter; 
	};

}
