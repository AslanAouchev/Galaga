#pragma once
#include "GameObjectCommand.h"
#include "PlayerComponent.h"
#include <SDLSoundSystem.h>
#include "SceneManager.h"
#include "BaseAIController.h"
#include "../Galaga/BossAIControllerComponent.h"

extern void loadMainMenu();

class MoveLeftCommand : public GameObjectCommand
{
private:
    float m_EventCooldown{ 0.0f };
    const float m_EventInterval{ 0.2f };

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
                    GetGameObject()->TriggerEvent("NameLeft");
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
                else
                {
                    m_EventCooldown -= deltaTime;
                    if (m_EventCooldown <= 0.0f)
                    {
                        GetGameObject()->TriggerEvent("NameLeft");
                        m_EventCooldown = m_EventInterval;
                    }
                }
            }
        }
    }
};

class MoveRightCommand : public GameObjectCommand
{
private:
    float m_EventCooldown{ 0.0f };
    const float m_EventInterval{ 0.2f };

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
                    GetGameObject()->TriggerEvent("NameRight");
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
                else
                {
                    m_EventCooldown -= deltaTime;
                    if (m_EventCooldown <= 0.0f)
                    {
                        GetGameObject()->TriggerEvent("NameRight");
                        m_EventCooldown = m_EventInterval;
                    }
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

class MuteCommand : public GameObjectCommand
{
public:

    MuteCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
		if (GetGameObject())
		{
			ServiceLocator::getSoundSystem().mute();
		}
    }

    virtual void Execute(float) override
    {

    }
};

class SkipLevelCommand : public GameObjectCommand
{
public:

    SkipLevelCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        GetGameObject()->TriggerEvent("SkipLevel");
    }

    virtual void Execute(float) override
    {

    }
};

class ShootAICOmmand : public GameObjectCommand
{
public:

    ShootAICOmmand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
		GetGameObject()->GetComponent<BossAIControllerComponent>()->ShootFighter();
    }

    virtual void Execute(float) override
    {

    }
};

class DiveAICOmmand : public GameObjectCommand
{
public:

    DiveAICOmmand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        GetGameObject()->GetComponent<BaseAIController>()->SetAttack(true);
    }

    virtual void Execute(float) override
    {

    }
};

class BeamAICOmmand : public GameObjectCommand
{
public:

    BeamAICOmmand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        GetGameObject()->GetComponent<BossAIControllerComponent>()->StartTractorBeam();
    }

    virtual void Execute(float) override
    {

    }
};