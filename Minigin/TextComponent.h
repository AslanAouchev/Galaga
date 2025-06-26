#pragma once
#include <string>
#include <memory>
#include "GameObject.h"
#include "Component.h"

namespace dae
{
	class Font;
	class Texture2D;
	class TextComponent : public Component
	{
	public:
		void Update(const float deltaTime) override;
		void Render() const override;

		void SetText(const std::string& text);

		TextComponent(const std::string& text, std::shared_ptr<Font> font, GameObject* pOwner);
		virtual ~TextComponent() = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;
	private:
		bool m_needsUpdate{};
		std::string m_text{};
		std::shared_ptr<Font> m_font{nullptr};
		std::shared_ptr<Texture2D> m_textTexture{nullptr};
	};
}
