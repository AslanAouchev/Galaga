#pragma once
#include "GameObjectCommand.h"
#include "PlayerComponent.h"
#include <SDLSoundSystem.h>
#include "SceneManager.h"

extern void loadMainMenu();

class MoveLeftCommand : public GameObjectCommand
{
public:
    MoveLeftCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            if (auto playerComp{ GetGameObject()->GetComponent<dae::PlayerComponent>() })
            {
                if(!playerComp->GetPaused() && !playerComp->GetKilledPaused())
                {
                    const auto transform{ GetGameObject()->GetTransform() };
                    const auto pos{ transform.GetPosition() };
                    GetGameObject()->SetPosition(pos.x - 200.f, pos.y);
                }
            }
        }
    }

    virtual void Execute(float deltaTime) override
    {
        if (GetGameObject())
        {
            if (auto playerComp{ GetGameObject()->GetComponent<dae::PlayerComponent>() })
            {
                if (!playerComp->GetPaused() && !playerComp->GetKilledPaused())
                {
                    const auto transform{ GetGameObject()->GetTransform() };
                    const auto pos{ transform.GetPosition() };
                    GetGameObject()->SetPosition(pos.x - 200.f * deltaTime, pos.y);
                }
            }
        }
    }
};

class MoveRightCommand : public GameObjectCommand
{
public:
    MoveRightCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            if (auto playerComp{ GetGameObject()->GetComponent<dae::PlayerComponent>() })
            {
                if (!playerComp->GetPaused() && !playerComp->GetKilledPaused())
                {
                    const auto transform{ GetGameObject()->GetTransform() };
                    const auto pos{ transform.GetPosition() };
                    GetGameObject()->SetPosition(pos.x + 200.f, pos.y);
                }
            }
        }
    }

    virtual void Execute(float deltaTime) override
    {
        if (GetGameObject())
        {
            if (auto playerComp{ GetGameObject()->GetComponent<dae::PlayerComponent>() })
            {
                if (!playerComp->GetPaused() && !playerComp->GetKilledPaused())
                {
                    const auto transform{ GetGameObject()->GetTransform() };
                    const auto pos{ transform.GetPosition() };
                    GetGameObject()->SetPosition(pos.x + 200.f * deltaTime, pos.y);
                }
            }
        }
    }
};

class FireCommand : public GameObjectCommand
{
public:

    FireCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            if (auto playerComp{ GetGameObject()->GetComponent<dae::PlayerComponent>() })
            {
                if(playerComp->Fire())
                {
                    ServiceLocator::getSoundSystem().play(13, 0.8f);
                }
            }
        }
    }

    virtual void Execute(float) override
    {

    }
};

class PauseCommand : public GameObjectCommand
{
public:

    PauseCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
			GetGameObject()->TriggerEvent("Pause");
        }
    }

    virtual void Execute(float) override
    {

    }
};

class UpUiCommand : public GameObjectCommand
{
public:

    UpUiCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            GetGameObject()->TriggerEvent("MenuUp");
        }
    }

    virtual void Execute(float) override
    {

    }
};

class DownUiCommand : public GameObjectCommand
{
public:

    DownUiCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            GetGameObject()->TriggerEvent("MenuDown");
        }
    }

    virtual void Execute(float) override
    {

    }
};

class ConfirmUiCommand : public GameObjectCommand
{
public:

    ConfirmUiCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            GetGameObject()->TriggerEvent("MenuConfirm");
        }
    }

    virtual void Execute(float) override
    {

    }
};

class ReturnToMenuCommand : public GameObjectCommand
{
public:

    ReturnToMenuCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            loadMainMenu();
			auto& sceneManager = dae::SceneManager::GetInstance();
            sceneManager.SetActiveScene("MainMenu");
        }
    }

    virtual void Execute(float) override
    {

    }
};