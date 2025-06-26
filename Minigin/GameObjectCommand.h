#pragma once

#include "Command.h"
#include "GameObject.h"

class GameObjectCommand : public Command
{
protected:
    dae::GameObject* m_pGameObject{nullptr};

    dae::GameObject* GetGameObject() const { return m_pGameObject; }

public:
    GameObjectCommand(dae::GameObject* gameObject) : m_pGameObject(gameObject) {}
    virtual ~GameObjectCommand() = default;
};