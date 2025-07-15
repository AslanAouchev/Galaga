#pragma once
#include "Component.h"
#include "Observer.h"
#include <glm.hpp>
#include "Font.h"

class MenuItemComponent : public dae::Component, public Observer
{
public:
    MenuItemComponent(dae::GameObject* owner, int itemIndex, const std::string& titleText, std::shared_ptr<dae::Font> font);
    ~MenuItemComponent() = default;

    MenuItemComponent(const MenuItemComponent& other) = delete;
    MenuItemComponent(MenuItemComponent&& other) = delete;
    MenuItemComponent& operator=(const MenuItemComponent& other) = delete;
    MenuItemComponent& operator=(MenuItemComponent&& other) = delete;

    void Update(float deltaTime) override;
    void Render() const override;
    void OnNotify(const EventData& event) override;

    int GetItemIndex() const { return m_ItemIndex; }
    bool IsSelected() const { return m_IsSelected; }
    void SetSelected(bool selected);
    void HideToggle();

private:
    int m_ItemIndex;
	bool m_HideToggle{ false };
    bool m_IsSelected{ false };
    std::string m_OriginalText;

    float m_BlinkTimer{ 0.0f };
    const float m_BlinkInterval{ 0.5f };
    bool m_ShowBlink{ true };

    void UpdateVisualFeedback(float deltaTime);
    void UpdateTextDisplay();
};