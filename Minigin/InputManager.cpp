#include <SDL.h>
#include "InputManager.h"
#include <iostream>
#include <unordered_map>

class dae::InputManager::InputManagerImpl
{
public:
    InputManagerImpl() : m_pController{ nullptr, SDL_GameControllerClose } {}

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
                const auto it{ m_KeyboardCommands.find(e.key.keysym.scancode) };
                if (it != m_KeyboardCommands.end() && it->second)
                {
                    it->second->Execute();
                }
            }
            else if (e.type == SDL_CONTROLLERBUTTONDOWN)
            {
                const auto it{ m_ControllerCommands.find(e.cbutton.button) };
                if (it != m_ControllerCommands.end() && it->second)
                {
                    it->second->Execute();
                }
            }
            else if (e.type == SDL_CONTROLLERDEVICEADDED)
            {
                HandleControllerConnection(e.cdevice.which);
            }
            else if (e.type == SDL_CONTROLLERDEVICEREMOVED)
            {
                HandleControllerDisconnection();
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

        if (m_pController)
        {
            for (const auto& pair : m_ContinuousControllerCommands)
            {
                if (SDL_GameControllerGetButton(m_pController.get(), static_cast<SDL_GameControllerButton>(pair.first)) && pair.second)
                {
                    pair.second->Execute(deltaTime);
                }
            }
        }
    }

    void BindCommand(SDL_Scancode key, std::unique_ptr<Command> command)
    {
        m_KeyboardCommands[key] = std::move(command);
    }

    void BindCommand(Uint8 controllerButton, std::unique_ptr<Command> command)
    {
        m_ControllerCommands[controllerButton] = std::move(command);
    }

    void BindContinuousCommand(SDL_Scancode key, std::unique_ptr<Command> command)
    {
        m_ContinuousKeyboardCommands[key] = std::move(command);
    }

    void BindContinuousCommand(Uint8 controllerButton, std::unique_ptr<Command> command)
    {
        m_ContinuousControllerCommands[controllerButton] = std::move(command);
    }

    void ClearBindings()
    {
        m_KeyboardCommands.clear();
        m_ControllerCommands.clear();
        m_ContinuousKeyboardCommands.clear();
        m_ContinuousControllerCommands.clear();
    }

private:
    void HandleControllerConnection(int deviceIndex)
    {
        if (!m_pController && SDL_IsGameController(deviceIndex))
        {
            m_pController.reset(SDL_GameControllerOpen(deviceIndex));
            if (m_pController)
            {
                std::cout << "Controller connected\n";
            }
        }
    }

    void HandleControllerDisconnection()
    {
        if (m_pController)
        {
            m_pController.reset();
            std::cout << "Controller disconnected\n";
        }
    }

    std::unordered_map<SDL_Scancode, std::unique_ptr<Command>> m_KeyboardCommands;
    std::unordered_map<Uint8, std::unique_ptr<Command>> m_ControllerCommands;

    std::unordered_map<SDL_Scancode, std::unique_ptr<Command>> m_ContinuousKeyboardCommands;
    std::unordered_map<Uint8, std::unique_ptr<Command>> m_ContinuousControllerCommands;

    std::unique_ptr<SDL_GameController, decltype(&SDL_GameControllerClose)> m_pController;
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

void dae::InputManager::BindCommand(Uint8 controllerButton, std::unique_ptr<Command> command)
{
    m_pImpl->BindCommand(controllerButton, std::move(command));
}

void dae::InputManager::BindContinuousCommand(SDL_Scancode key, std::unique_ptr<Command> command)
{
    m_pImpl->BindContinuousCommand(key, std::move(command));
}

void dae::InputManager::BindContinuousCommand(Uint8 controllerButton, std::unique_ptr<Command> command)
{
    m_pImpl->BindContinuousCommand(controllerButton, std::move(command));
}

void dae::InputManager::ClearBindings()
{
    m_pImpl->ClearBindings();
}