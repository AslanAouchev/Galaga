#include <SDL.h>
#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "Scene.h"
#include "TextureComponent.h"
#include "FpsComponent.h"
#include "MovementComponent.h"
#include <iostream>
#include "PlayerComponent.h"
#include "InputManager.h"
#include "InputCommands.h"
#include <BackgroundScrollComponent.h>
#include <BoundsComponent.h>
#include <BeeAiControllerComponent.h>
#include "GalagaGameManager.h"

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

	auto& input = dae::InputManager::GetInstance();

	int numJoysticks = SDL_NumJoysticks();
	std::cout << "Number of joysticks detected: " << numJoysticks << std::endl;

	for (int i = 0; i < numJoysticks; i++) {
		if (SDL_IsGameController(i)) {
			std::cout << "Controller " << i << " is supported" << std::endl;
		}
	}

	auto fo = std::make_unique<dae::GameObject>();
	fo->AddComponent<GalagaGameManager>(fo.get());

	auto bg1 = std::make_unique<dae::GameObject>();
	bg1->AddComponent<dae::TextureComponent>("background.png", bg1.get());
	bg1->AddComponent<dae::BackgroundScrollComponent>(bg1.get(), 80.f, 480.f, 0.f);
	bg1->SetPosition(0, 0);

	fo->AddObserver(bg1.get()->GetComponent<Observer>());

	scene.Add(std::move(bg1));

	auto bg2 = std::make_unique<dae::GameObject>();
	bg2->AddComponent<dae::TextureComponent>("background.png", bg2.get());
	bg2->AddComponent<dae::BackgroundScrollComponent>(bg2.get(), 80.f, 480.f, -480.f);
	bg2->SetPosition(0,-480);

	fo->AddObserver(bg2.get()->GetComponent<Observer>());

	scene.Add(std::move(bg2));

	auto go = std::make_unique<dae::GameObject>();

	go = std::make_unique<dae::GameObject>();
	go->SetPosition(300, 400);
	go->AddComponent<dae::PlayerComponent>(go.get(), "galaga.png", "Bullet.png", 10);
	go->AddComponent<dae::BoundsComponent>(go.get(), 640.f, 480.f);
	input.BindContinuousCommand(SDL_SCANCODE_A, std::make_unique<MoveLeftCommand>(go.get()));
	input.BindContinuousCommand(SDL_SCANCODE_D, std::make_unique<MoveRightCommand>(go.get()));
	input.BindCommand(SDL_SCANCODE_SPACE, std::make_unique<FireCommand>(go.get()));
	input.BindCommand(SDL_SCANCODE_ESCAPE, std::make_unique<PauseCommand>(go.get()));
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_LEFT, std::make_unique<MoveLeftCommand>(go.get()));
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, std::make_unique<MoveRightCommand>(go.get()));
	input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<FireCommand>(go.get()));

	go->AddObserver(fo.get()->GetComponent<Observer>());
	fo->AddObserver(go.get()->GetComponent<Observer>());

	scene.Add(std::move(go));

	for (int i{}; i < 10; i++)
	{
		go = std::make_unique<dae::GameObject>();
		go->SetPosition(static_cast<float>(150 + i * 40), 100);
		go->AddComponent<BeeAiControllerComponent>(go.get());

		go->SetParent(fo.get(), true);

		fo->AddObserver(go.get()->GetComponent<BeeAiControllerComponent>());

		scene.Add(std::move(go));
	}

	scene.Add(std::move(fo));

	auto& soundSystem = ServiceLocator::getSoundSystem();

	soundSystem.registerSound(0, "../Data/Bonus_Stage.wav");
	soundSystem.registerSound(1, "../Data/Bonus_Stage_Perfect.wav");
	soundSystem.registerSound(2, "../Data/Bonus_Stage_Result.wav");
	soundSystem.registerSound(3, "../Data/Captured_Fighter_Destroyed.wav");
	soundSystem.registerSound(4, "../Data/Coin_Insert.wav");
	soundSystem.registerSound(5, "../Data/Enemy_Hit.wav");
	soundSystem.registerSound(6, "../Data/High_Score.wav");
	soundSystem.registerSound(7, "../Data/High_Score_5thplace.wav");
	soundSystem.registerSound(8, "../Data/Last_Ship_Destroyed.wav");
	soundSystem.registerSound(9, "../Data/Level_Flag.wav");
	soundSystem.registerSound(10, "../Data/Life_Up.wav");
	soundSystem.registerSound(11, "../Data/Opening_Theme.wav");
	soundSystem.registerSound(12, "../Data/Ship_Captured_Rescued.wav");
	soundSystem.registerSound(13, "../Data/Shoot.wav");
	soundSystem.registerSound(14, "../Data/Tractor_Beam.wav");
}

int main(int, char* [])
{
	dae::Minigin engine("../Data/");
	engine.Run(load);

	return 0;
}