#include "SceneManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include <stdexcept>

void dae::SceneManager::Update(const float deltaTime)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(deltaTime);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	for (auto& scene : m_scenes)
	{
		if (scene && scene->GetName() == name)
		{
			if (m_activeScene == scene.get())
			{
				m_activeScene = nullptr;
			}

			auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
			if (it != m_scenes.end())
			{
				m_scenes.erase(it);
			}
			break;
		}
	}

	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_scenes.push_back(scene);
	if (!m_activeScene)
	{
		SetActiveScene(name);
	}
	return *scene;
}

dae::Scene& dae::SceneManager::GetActiveScene()
{
	if (!m_activeScene && !m_scenes.empty())
	{
		m_activeScene = m_scenes[0].get();
	}

	if (!m_activeScene)
	{
		throw std::runtime_error("no scene");
	}

	return *m_activeScene;
}

dae::Scene* dae::SceneManager::GetScene(const std::string& name)
{
	for (auto& scene : m_scenes)
	{
		if (scene->GetName() == name)
		{
			return scene.get();
		}
	}
	return nullptr;
}

void dae::SceneManager::SetActiveScene(const std::string& name)
{
	for (auto& scene : m_scenes)
	{
		if (scene->GetName() == name)
		{
			m_activeScene = scene.get();
			return;
		}
	}
}

void dae::SceneManager::RemoveAllScenes()
{
	for (auto& scene : m_scenes)
	{
		if (scene)
		{
			scene->RemoveAll();
		}
	}
	m_activeScene = nullptr;
}
