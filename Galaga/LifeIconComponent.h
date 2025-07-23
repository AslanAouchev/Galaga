#pragma once
#include "Component.h"
#include "Observer.h"

class LifeIconComponent : public dae::Component, public Observer
{
public:
    LifeIconComponent(dae::GameObject* owner, int iconIndex);
    ~LifeIconComponent();

    LifeIconComponent(const LifeIconComponent& other) = delete;
    LifeIconComponent(LifeIconComponent&& other) = delete;
    LifeIconComponent& operator=(const LifeIconComponent& other) = delete;
    LifeIconComponent& operator=(LifeIconComponent&& other) = delete;

    virtual void Update(float) override {}
    virtual void Render() const override {}
    void OnNotify(const EventData& event) override;

private:
    int m_IconIndex{};
    bool m_IsVisible{ true };

    void ShowIcon();
    void HideIcon();
};