#include "FpsComponent.h"

dae::FpsComponent::FpsComponent(std::shared_ptr<Font> font, GameObject* pOwner):
	Component(pOwner)
{
	m_pText = std::make_unique<TextComponent>("text", std::move(font), GetOwner());
}

void dae::FpsComponent::Render() const
{
	m_pText->Render();
}

void dae::FpsComponent::Update(const float deltaTime)
{
	std::string frameString{ std::to_string(1 / deltaTime) };

	m_pText->SetText(frameString);

	m_pText->Update(deltaTime);
}
