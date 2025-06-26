#pragma once
#include "GameObjectCommand.h"
#include "PlayerComponent.h"
#include <SDLSoundSystem.h>

class MoveLeftCommand : public GameObjectCommand
{
public:
    MoveLeftCommand(dae::GameObject* gameObject) : GameObjectCommand(gameObject) {}

    virtual void Execute() override
    {
        if (GetGameObject())
        {
            const auto transform{ GetGameObject()->GetTransform() };
            const auto pos{ transform.GetPosition() };
            GetGameObject()->SetPosition(pos.x - 200.f, pos.y);
        }
    }

    virtual void Execute(float deltaTime) override
    {
        if (GetGameObject())
        {
            const auto transform{ GetGameObject()->GetTransform() };
            const auto pos{ transform.GetPosition() };
            GetGameObject()->SetPosition(pos.x - 200.f * deltaTime, pos.y);
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
            const auto transform{ GetGameObject()->GetTransform() };
            const auto pos{ transform.GetPosition() };
            GetGameObject()->SetPosition(pos.x + 200.f, pos.y);
        }
    }

    virtual void Execute(float deltaTime) override
    {
        if (GetGameObject())
        {
            const auto transform{ GetGameObject()->GetTransform() };
            const auto pos{ transform.GetPosition() };
            GetGameObject()->SetPosition(pos.x + 200.f * deltaTime, pos.y);
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