#include "Scene.h"
#include <algorithm>
#include "PlayerComponent.h"
#include "../Galaga/GalagaGameManager.h"
#include "../Galaga/MenuManager.h"
#include "BaseAIController.h"

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
        CleanupObjectObserverRelationships(object);
        object->MarkForDestruction();
    }
}

void Scene::RemoveAll()
{
	m_shouldRemoveAll = true;
}

void Scene::Update(const float& deltaTime)
{
    for (int i{}; i < m_objects.size(); ++i)
    {
        if (m_objects[i])
        {
            m_objects[i]->Update(deltaTime);
        }
    }

    if (m_shouldRemoveAll)
    {
        for (auto& obj : m_objects)
        {
            if (obj)
            {
                CleanupObjectObserverRelationships(obj.get());
            }
        }
        m_objects.clear();
        m_shouldRemoveAll = false;
    }
    else
    {
        for (auto& obj : m_objects)
        {
            if (obj && obj->IsMarkedForDestruction())
            {
                CleanupObjectObserverRelationships(obj.get());
            }
        }

        m_objects.erase(
            std::remove_if(m_objects.begin(), m_objects.end(),
                [](const std::unique_ptr<GameObject>& obj) {
                    return !obj || obj->IsMarkedForDestruction();
                }),
            m_objects.end());
    }
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

void Scene::CleanupObjectObserverRelationships(GameObject* objectToRemove)
{
    if (!objectToRemove) return;

    for (auto& otherObject : m_objects)
    {
        if (otherObject && otherObject.get() != objectToRemove)
        {
            CleanupComponentObservers(objectToRemove, otherObject.get());
        }
    }
}

void Scene::CleanupComponentObservers(GameObject* objectToRemove, GameObject* otherObject)
{
    auto playerComp{ objectToRemove->GetComponent<dae::PlayerComponent>() };
    if (playerComp)
    {
        otherObject->RemoveObserver(playerComp);
    }

	for(auto& component : objectToRemove->GetAllComponents())
	{
        if (auto* observer = dynamic_cast<Observer*>(component))
        {
            otherObject->RemoveObserver(observer);
        }
	}
}