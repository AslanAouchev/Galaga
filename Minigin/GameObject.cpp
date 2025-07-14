#include "GameObject.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"

dae::GameObject::~GameObject()
{	
	if (m_Parent)
	{
		m_Parent->RemoveChild(this);
	}

	for (auto* child : m_Children)
	{
		if (child)
		{
			child->m_Parent = nullptr; 
		}
	}
	m_Children.clear();
	m_Observers.clear();
}
void dae::GameObject::Update(const float deltaTime)
{
	if (!m_IsActive) return;

	for (auto& components : m_Components)
	{
		if(components)
		{
			components->Update(deltaTime);
		}
	}

	UpdateWorldPosition();
}

void dae::GameObject::Render() const
{
	if (!m_IsActive) return;

	for (const auto& components : m_Components)
	{
		if(components)
		{
			components->Render();
		}
	}
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_LocalPosition = glm::vec3(x, y, 0.0f);
	m_transform.SetPosition(x, y, 0.0f);
	SetPositionDirty();
}

void dae::GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
{
	if (IsChild(pParent) || pParent == this || m_Parent == pParent)
		return;

	auto oldParent = m_Parent;

	if (pParent == nullptr)
		SetLocalPosition(GetWorldPosition());
	else
	{
		if (keepWorldPosition)
			SetLocalPosition(GetWorldPosition() - pParent->GetWorldPosition());
		SetPositionDirty();
	}

	if (oldParent)
		oldParent->RemoveChild(this);

	m_Parent = pParent;

	if (pParent)
		pParent->AddChild(this);
}

void dae::GameObject::RemoveComponent(Component* components)
{
	const auto it = std::find_if(m_Components.begin(), m_Components.end(),
		[components](const std::unique_ptr<Component>& comp) {
			return comp.get() == components;
		});

	if (it != m_Components.end())
	{
		m_Components.erase(it);
	}
}

void dae::GameObject::AddChild(GameObject* pChild)
{
	m_Children.push_back(pChild);
}

void dae::GameObject::RemoveChild(GameObject* pChildToRemove)
{
	const auto it = std::find_if(m_Children.begin(), m_Children.end(),
		[pChildToRemove](const GameObject* child) {
			return child == pChildToRemove;
		});

	if (it != m_Children.end())
	{
		m_Children.erase(it);
	}
}

bool dae::GameObject::IsChild(GameObject* pChildToCompare)
{
	const auto it = std::find_if(m_Children.begin(), m_Children.end(),
		[pChildToCompare](const GameObject* child) {
			return child == pChildToCompare;
		});

	if (it != m_Children.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

glm::vec3 dae::GameObject::GetWorldPosition()
{
	if (m_positionIsDirty) UpdateWorldPosition();
	return m_WorldPosition;
}

void dae::GameObject::UpdateWorldPosition()
{
	if (m_positionIsDirty)
	{
		if (m_Parent == nullptr)
			m_WorldPosition = m_LocalPosition;
		else
			m_WorldPosition = m_Parent->GetWorldPosition() + m_LocalPosition;

		m_transform.SetPosition(m_WorldPosition.x, m_WorldPosition.y, m_WorldPosition.z);
	}
	m_positionIsDirty = false;
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_LocalPosition = pos;
	SetPositionDirty();
}

void dae::GameObject::SetPositionDirty()
{
	if (m_positionIsDirty) return;

	m_positionIsDirty = true;

	for(const auto& children : m_Children)
	{
		children->SetPositionDirty();
	}
}
