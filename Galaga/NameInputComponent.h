#pragma once
#include "Component.h"
#include "Observer.h"
#include <string>
#include <memory>

namespace dae
{
    class Font;
    class TextComponent;
}

class NameInputComponent : public dae::Component, public Observer
{
public:
    NameInputComponent(dae::GameObject* owner, int score, int shots, int enemies, float accuracy);
    ~NameInputComponent() = default;
    NameInputComponent(const NameInputComponent& other) = delete;
    NameInputComponent(NameInputComponent&& other) = delete;
    NameInputComponent& operator=(const NameInputComponent& other) = delete;
    NameInputComponent& operator=(NameInputComponent&& other) = delete;

    virtual void Update(float deltaTime) override;
    virtual void Render() const override {};
    void OnNotify(const EventData& event) override;

    const std::string& GetCurrentName() const { return m_CurrentName; }
    bool IsActive() const { return m_IsActive; }
    void SetActive(bool active);

private:
    int m_Score;
    int m_ShotsTotal;
    int m_EnemiesKilled;
    float m_Accuracy;

    std::string m_CurrentName{ "AAA" };
    int m_NamePosition{ 0 };
    bool m_IsActive{ false };

    float m_BlinkTimer{ 0.0f };
    bool m_ShowBlink{ true };
    bool m_HideToggle{ false };
    const float m_BlinkInterval{ 0.5f };

    std::shared_ptr<dae::Font> m_Font;

    dae::GameObject* m_TitleObject{ nullptr };
    dae::GameObject* m_ScoreObject{ nullptr };
    dae::GameObject* m_StatsObject{ nullptr };
    dae::GameObject* m_NameObject{ nullptr };
    dae::GameObject* m_InstructionObject{ nullptr };

    void UpdateNameDisplay();
    void UpdateVisualFeedback(float deltaTime);
    void CreateTextComponents();
    void ShowTextObjects();
    void HideTextObjects();
};