#include <SDL.h>
#include "InputManager.h"
#include <iostream>
#include <unordered_map>
#include <array>

class dae::InputManager::InputManagerImpl
{
public:
    InputManagerImpl()
    {
        for (auto& controller : m_Controllers)
        {
            controller.gameController = { nullptr, SDL_GameControllerClose };
            controller.isConnected = false;
            controller.joystickId = -1;
        }
    }

    bool ProcessInput()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                return false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                const auto it = m_KeyboardCommands.find(e.key.keysym.scancode);
                if (it != m_KeyboardCommands.end() && it->second)
                {
                    it->second->Execute();
                }
            }
            else if (e.type == SDL_CONTROLLERBUTTONDOWN)
            {
                int controllerIndex = FindControllerByJoystickId(e.cbutton.which);
                if (controllerIndex != -1)
                {
                    const auto it = m_ControllerCommands[controllerIndex].find(e.cbutton.button);
                    if (it != m_ControllerCommands[controllerIndex].end() && it->second)
                    {
                        it->second->Execute();
                    }
                }
            }
            else if (e.type == SDL_CONTROLLERDEVICEADDED)
            {
                HandleControllerConnection(e.cdevice.which);
            }
            else if (e.type == SDL_CONTROLLERDEVICEREMOVED)
            {
                HandleControllerDisconnection(e.cdevice.which);
            }
        }
        return true;
    }

    void Update(float deltaTime)
    {
        const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
        for (const auto& pair : m_ContinuousKeyboardCommands)
        {
            if (keyboardState[pair.first] && pair.second)
            {
                pair.second->Execute(deltaTime);
            }
        }

        for (int i{}; i < MAX_CONTROLLERS; ++i)
        {
            if (m_Controllers[i].isConnected && m_Controllers[i].gameController)
            {
                for (const auto& pair : m_ContinuousControllerCommands[i])
                {
                    if (SDL_GameControllerGetButton(m_Controllers[i].gameController.get(),
                        static_cast<SDL_GameControllerButton>(pair.first)) && pair.second)
                    {
                        pair.second->Execute(deltaTime);
                    }
                }
            }
        }
    }

    void BindCommand(SDL_Scancode key, std::unique_ptr<Command> command)
    {
        m_KeyboardCommands[key] = std::move(command);
    }

    void BindCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex = 0)
    {
        if (IsValidControllerIndex(controllerIndex))
        {
            m_ControllerCommands[controllerIndex][controllerButton] = std::move(command);
        }
    }

    void BindContinuousCommand(SDL_Scancode key, std::unique_ptr<Command> command)
    {
        m_ContinuousKeyboardCommands[key] = std::move(command);
    }

    void BindContinuousCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex = 0)
    {
        if (IsValidControllerIndex(controllerIndex))
        {
            m_ContinuousControllerCommands[controllerIndex][controllerButton] = std::move(command);
        }
    }

    void ClearBindings()
    {
        m_KeyboardCommands.clear();
        m_ContinuousKeyboardCommands.clear();

        for (int i{}; i < MAX_CONTROLLERS; ++i)
        {
            m_ControllerCommands[i].clear();
            m_ContinuousControllerCommands[i].clear();
        }
    }

private:
    static constexpr int MAX_CONTROLLERS{2};

    struct ControllerData
    {
        std::unique_ptr<SDL_GameController, decltype(&SDL_GameControllerClose)> gameController;
        bool isConnected;
        SDL_JoystickID joystickId;

        ControllerData()
            : gameController(nullptr, SDL_GameControllerClose)
            , isConnected(false)
            , joystickId(-1)
        {
        }
    };

    bool IsValidControllerIndex(int index) const
    {
        return index >= 0 && index < MAX_CONTROLLERS;
    }

    int FindControllerByJoystickId(SDL_JoystickID joystickId) const
    {
        for (int i{}; i < MAX_CONTROLLERS; ++i)
        {
            if (m_Controllers[i].isConnected && m_Controllers[i].joystickId == joystickId)
            {
                return i;
            }
        }
        return -1;
    }

    int FindFirstAvailableControllerSlot() const
    {
        for (int i = 0; i < MAX_CONTROLLERS; ++i)
        {
            if (!m_Controllers[i].isConnected)
            {
                return i;
            }
        }
        return -1;
    }

    void HandleControllerConnection(int deviceIndex)
    {
        if (!SDL_IsGameController(deviceIndex))
        {
            return;
        }

        int availableSlot{ FindFirstAvailableControllerSlot() };
        if (availableSlot == -1)
        {
            return;
        }

        SDL_GameController* controller{ SDL_GameControllerOpen(deviceIndex) };
        if (!controller)
        {
            return;
        }

        SDL_Joystick* joystick{ SDL_GameControllerGetJoystick(controller) };
        if (!joystick)
        {
            SDL_GameControllerClose(controller);
            return;
        }

        SDL_JoystickID joystickId{ SDL_JoystickInstanceID(joystick) };

        m_Controllers[availableSlot].gameController.reset(controller);
        m_Controllers[availableSlot].isConnected = true;
        m_Controllers[availableSlot].joystickId = joystickId;
    }

    void HandleControllerDisconnection(SDL_JoystickID joystickId)
    {
        int controllerIndex{ FindControllerByJoystickId(joystickId) };
        if (controllerIndex != -1)
        {
            m_Controllers[controllerIndex].gameController.reset();
            m_Controllers[controllerIndex].isConnected = false;
            m_Controllers[controllerIndex].joystickId = -1;
        }
    }

    std::unordered_map<SDL_Scancode, std::unique_ptr<Command>> m_KeyboardCommands;
    std::unordered_map<SDL_Scancode, std::unique_ptr<Command>> m_ContinuousKeyboardCommands;

    std::array<std::unordered_map<Uint8, std::unique_ptr<Command>>, MAX_CONTROLLERS> m_ControllerCommands;
    std::array<std::unordered_map<Uint8, std::unique_ptr<Command>>, MAX_CONTROLLERS> m_ContinuousControllerCommands;

    std::array<ControllerData, MAX_CONTROLLERS> m_Controllers;
};

dae::InputManager::InputManager() : m_pImpl(std::make_unique<InputManagerImpl>()) {}

dae::InputManager::~InputManager() = default;

bool dae::InputManager::ProcessInput()
{
    return m_pImpl->ProcessInput();
}

void dae::InputManager::Update(float deltaTime)
{
	m_pImpl->Update(deltaTime);
}

void dae::InputManager::BindCommand(SDL_Scancode key, std::unique_ptr<Command> command)
{
    m_pImpl->BindCommand(key, std::move(command));
}

void dae::InputManager::BindCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex)
{
    m_pImpl->BindCommand(controllerButton, std::move(command), controllerIndex);
}

void dae::InputManager::BindContinuousCommand(SDL_Scancode key, std::unique_ptr<Command> command)
{
    m_pImpl->BindContinuousCommand(key, std::move(command));
}

void dae::InputManager::BindContinuousCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex)
{
    m_pImpl->BindContinuousCommand(controllerButton, std::move(command), controllerIndex);
}

void dae::InputManager::ClearBindings()
{
    m_pImpl->ClearBindings();
}