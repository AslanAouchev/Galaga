#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& name);

		void Update(const float deltaTime);
		void Render();

		Scene& GetActiveScene();
		Scene* GetScene(const std::string& name);
		void SetActiveScene(const std::string& name);

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<Scene>> m_scenes;
		Scene* m_activeScene{ nullptr };
	};
}
