#pragma once
#include "Singleton.h"
#include "Command.h"
#include <memory>
#include <SDL.h>

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();
		~InputManager();

		bool ProcessInput();
		void Update(float deltaTime);

		void BindCommand(SDL_Scancode key, std::unique_ptr<Command> command);
		void BindCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex);

		void BindContinuousCommand(SDL_Scancode key, std::unique_ptr<Command> command);
		void BindContinuousCommand(Uint8 controllerButton, std::unique_ptr<Command> command, int controllerIndex);

		void BindContinuousAxisCommand(SDL_GameControllerAxis axis, std::unique_ptr<Command> command, bool positive, int controllerIndex);

		void ClearBindings();

	private:
		class InputManagerImpl;
		std::unique_ptr<InputManagerImpl> m_pImpl;
	};

}
