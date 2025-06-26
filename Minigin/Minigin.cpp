#include <stdexcept>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <thread>
#include "Minigin.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <iostream>
#include "SDLSoundSystem.h"

SDL_Window* g_window{};

void PrintSDLVersion()
{
	SDL_version version{};
	SDL_VERSION(&version);
	printf("We compiled against SDL version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	SDL_GetVersion(&version);
	printf("We are linking against SDL version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_IMAGE_VERSION(&version);
	printf("We compiled against SDL_image version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *IMG_Linked_Version();
	printf("We are linking against SDL_image version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_TTF_VERSION(&version);
	printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *TTF_Linked_Version(); 
	printf("We are linking against SDL_ttf version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_MIXER_VERSION(&version);
	printf("We compiled against SDL_mixer version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *Mix_Linked_Version();
	printf("We are linking against SDL_mixer version %u.%u.%u.\n",
		version.major, version.minor, version.patch);
}

dae::Minigin::Minigin(const std::string &dataPath)
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	g_window = SDL_CreateWindow(
		"Galaga",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);

	ResourceManager::GetInstance().Init(dataPath);

	try 
	{
		std::cout << "Initializing sound system..." << std::endl;

		ServiceLocator::initialize();

		auto sdlSystem = std::make_unique<SDLSoundSystem>();

#ifdef _DEBUG
		auto loggingSystem = std::make_unique<LoggingSoundSystem>(std::move(sdlSystem));
		ServiceLocator::registerSoundSystem(std::move(loggingSystem));
#else
		ServiceLocator::registerSoundSystem(std::move(sdlSystem));
#endif
		ServiceLocator::getSoundSystem().initialize();

		std::cout << "Sound system initilaised" << std::endl;
	}
	catch (const std::exception&) {
		std::cerr << "Sound system not initilaised";
	}
}

dae::Minigin::~Minigin()
{
	try 
	{
		ServiceLocator::shutdown();
		std::cout << "Sound system shut down" << std::endl;
	}
	catch (const std::exception& ) 
	{
		std::cerr << "Error during sound system shutdown" << std::endl;
	}

	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
	load();

	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = InputManager::GetInstance();

	bool doContinue = true;
	auto last_time = std::chrono::high_resolution_clock::now();
	float lag = 0.0f;
	while (doContinue)
	{
		using namespace std::chrono;

		const auto current_time = high_resolution_clock::now();
		const float delta_time = duration<float>(current_time - last_time).count();
		last_time = current_time;
		lag += delta_time;

		doContinue = input.ProcessInput();

		InputManager::GetInstance().Update(delta_time);

		sceneManager.Update(delta_time);
		renderer.Render();

		const auto frame_end_time = high_resolution_clock::now();
		const auto elapsed_time = duration<float, std::milli>(frame_end_time - current_time).count();
		const auto sleep_time = milliseconds(ms_per_frame) - milliseconds(static_cast<int>(elapsed_time));

		if (sleep_time.count() > 0)
		{
			std::this_thread::sleep_for(sleep_time);
		}
	}
}
