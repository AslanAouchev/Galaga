#include "TextureComponent.h"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "Renderer.h"

dae::TextureComponent::TextureComponent(const std::string& filename, GameObject* pOwner):
	Component(pOwner)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextureComponent::Update(const float)
{

}

void dae::TextureComponent::Render() const
{
	const auto& pos = GetOwner()->GetTransform().GetPosition();
	if (m_texture != nullptr)
	{
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

glm::ivec2 dae::TextureComponent::GetTextureSize() const
{
    if (m_texture != nullptr)
    {
        return m_texture->GetSize();
    }

    return glm::ivec2(0.0f, 0.0f);
}
