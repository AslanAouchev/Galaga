#pragma once
#include <memory>
#include "GameObject.h"
#include "Component.h"
#include "TextComponent.h"

class TextComponent;
namespace dae
{
	class FpsComponent final : public Component
	{
	public:
		void Update(const float deltaTime) override;
		void Render() const override;

		FpsComponent(std::shared_ptr<Font> font, GameObject* pOwner);
		virtual ~FpsComponent() = default;
		FpsComponent(const FpsComponent& other) = delete;
		FpsComponent(FpsComponent&& other) = delete;
		FpsComponent& operator=(const FpsComponent& other) = delete;
		FpsComponent& operator=(FpsComponent&& other) = delete;

	private:
		std::unique_ptr<TextComponent> m_pText{nullptr};
	};
}
