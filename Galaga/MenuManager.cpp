#include "MenuManager.h"
#include "SceneManager.h"
#include <iostream>
#include "GameObject.h"

extern void loadSinglePlayer();
extern void loadTwoPlayer();
extern void loadPvP();
extern void loadHighScores();

MenuManager::MenuManager(dae::GameObject* owner) : Component(owner)
{
}

void MenuManager::Update(float)
{
}

void MenuManager::Render() const
{
}

void MenuManager::OnNotify(const EventData& event)
{
    if (event.eventType == "MenuUp")
    {
        MoveUp();
    }
    else if (event.eventType == "MenuDown")
    {
        MoveDown();
    }
    else if (event.eventType == "MenuConfirm")
    {
        Confirm();
    }
}

void MenuManager::MoveUp()
{
    m_CurrentSelection = (m_CurrentSelection - 1 + m_MenuItemCount) % m_MenuItemCount;
    UpdateSelection();
}

void MenuManager::MoveDown()
{
    m_CurrentSelection = (m_CurrentSelection + 1) % m_MenuItemCount;
    UpdateSelection();
}

void MenuManager::UpdateSelection()
{
    GetOwner()->TriggerEvent("DeselectAll");

    GetOwner()->TriggerEvent("Select" + std::to_string(m_CurrentSelection));
}

void MenuManager::Confirm()
{
    switch (m_CurrentSelection)
    {
    case 0: LoadScene(GameScene::SinglePlayer); break;
    case 1: LoadScene(GameScene::TwoPlayer); break;
    case 2: LoadScene(GameScene::PvP); break;
	case 3: LoadScene(GameScene::HighScores); break;
    case 4: exit(0); break;
    }
}

void MenuManager::LoadScene(GameScene scene)
{
    auto& sceneManager = dae::SceneManager::GetInstance();

    switch (scene)
    {
    case GameScene::SinglePlayer:
        loadSinglePlayer();
        sceneManager.SetActiveScene("SinglePlayer");
        break;
    case GameScene::TwoPlayer:
        loadTwoPlayer();
        sceneManager.SetActiveScene("TwoPlayer");
        break;
    case GameScene::HighScores:
        loadHighScores();
        sceneManager.SetActiveScene("HighScore");
        break;
    case GameScene::PvP:
        loadPvP();
        sceneManager.SetActiveScene("PvP");
        break;
    }
}