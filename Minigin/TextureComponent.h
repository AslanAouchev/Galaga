#pragma once
#include <string>
#include <memory>
#include "GameObject.h"
#include "Component.h"

namespace dae
{
	class Texture2D;
	class TextureComponent final : public Component
	{
	public:
		void Update(const float deltaTime) override;
		void Render() const override;

		void SetTexture(const std::string& filename);
		glm::ivec2 GetTextureSize() const;

		TextureComponent(const std::string& filename, GameObject* pOwner);
		virtual ~TextureComponent() = default;
		TextureComponent(const TextureComponent& other) = delete;
		TextureComponent(TextureComponent&& other) = delete;
		TextureComponent& operator=(const TextureComponent& other) = delete;
		TextureComponent& operator=(TextureComponent&& other) = delete;
	private:
		std::shared_ptr<Texture2D> m_texture{nullptr};

	};
}
