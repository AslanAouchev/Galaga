#include "Scene.h"
#include <algorithm>

using namespace dae;

unsigned int Scene::m_idCounter = 0;

Scene::Scene(const std::string& name) : m_name(name) {}

Scene::~Scene() = default;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(GameObject* object)
{
	if (object)
	{
		object->MarkForDestruction();
	}
}

void Scene::RemoveAll()
{
	m_shouldRemoveAll = true;
}

void Scene::Update(const float& deltaTime)
{
	for(auto& object : m_objects)
	{
		object->Update(deltaTime);
	}

	if (m_shouldRemoveAll)
	{
		m_objects.clear();
		m_shouldRemoveAll = false;
	}

	m_objects.erase(
		std::remove_if(m_objects.begin(), m_objects.end(),
			[](const std::unique_ptr<GameObject>& obj) {
				return !obj || obj->IsMarkedForDestruction();
			}),
		m_objects.end());
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}