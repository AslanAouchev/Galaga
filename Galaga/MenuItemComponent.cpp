#include "MenuItemComponent.h"
#include "GameObject.h"
#include <iostream>
#include "ResourceManager.h"
#include <TextComponent.h>

MenuItemComponent::MenuItemComponent(dae::GameObject* owner, int itemIndex, const std::string& titleText, std::shared_ptr<dae::Font> font)
    : Component(owner), m_ItemIndex(itemIndex), m_OriginalText(titleText)
{
    GetOwner()->AddComponent<dae::TextComponent>(m_OriginalText, font, owner);
    if (m_ItemIndex == 0)
    {
		SetSelected(true);
    }
}

void MenuItemComponent::Update(float deltaTime)
{
    if (m_IsSelected)
    {
        UpdateVisualFeedback(deltaTime);
    }
}

void MenuItemComponent::Render() const
{

}

void MenuItemComponent::OnNotify(const EventData& event)
{
    if (event.eventType == "DeselectAll")
    {
        SetSelected(false);
    }
    else if (event.eventType == "PauseUI")
    {
        HideToggle();
    }
    else if (event.eventType == "Resume")
    {
        HideToggle();
    }
    else if (event.eventType == "Select" + std::to_string(m_ItemIndex))
    {
        SetSelected(true);
    }
}

void MenuItemComponent::SetSelected(bool selected)
{
    if (m_IsSelected != selected)
    {
        m_IsSelected = selected;
        m_BlinkTimer = 0.0f;
        m_ShowBlink = true;
        UpdateTextDisplay();
    }
}

void MenuItemComponent::HideToggle()
{
	m_HideToggle = !m_HideToggle;
    GetOwner()->SetPosition(GetOwner()->GetTransform().GetPosition().x, GetOwner()->GetTransform().GetPosition().y + (m_HideToggle ? 1 : -1) * 10000);
}

void MenuItemComponent::UpdateVisualFeedback(float deltaTime)
{
    m_BlinkTimer += deltaTime;
    if (m_BlinkTimer >= m_BlinkInterval)
    {
        m_ShowBlink = !m_ShowBlink;
        m_BlinkTimer = 0.0f;
        UpdateTextDisplay();
    }
}

void MenuItemComponent::UpdateTextDisplay()
{
    std::string displayText = m_OriginalText;

    if (m_IsSelected)
    {
        if (m_ShowBlink)
        {
            displayText = "> " + m_OriginalText + " <";
        }
        else
        {
            displayText = "  " + m_OriginalText + "  ";
        }
    }

    GetOwner()->GetComponent<dae::TextComponent>()->SetText(displayText);
}