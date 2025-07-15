#pragma once
#include "Component.h"
#include "Observer.h"
#include "MenuItemComponent.h"

enum class GameScene
{
    SinglePlayer,
    TwoPlayer,
    PvP,
	HighScores
};

class MenuManager : public dae::Component, public Observer
{
public:
    MenuManager(dae::GameObject* owner);
    ~MenuManager() = default;

    void Update(float deltaTime) override;
    void Render() const override;
    void OnNotify(const EventData& event) override;

    void MoveUp();
    void MoveDown();
    void UpdateSelection();
    void Confirm();

private:
    int m_CurrentSelection{ 0 };
    int m_MenuItemCount{ 5 };

    void LoadScene(GameScene scene);
};