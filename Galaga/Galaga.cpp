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
#include "MenuManager.h"
#include "MenuItemComponent.h"
#include "HighScoreManager.h"

void loadMainMenu()
{
	ServiceLocator::getSoundSystem().stopAllSounds();

	dae::SceneManager::GetInstance().RemoveAllScenes();

	auto& scene = dae::SceneManager::GetInstance().CreateScene("MainMenu");
	dae::SceneManager::GetInstance().SetActiveScene("MainMenu");
	auto& input = dae::InputManager::GetInstance();

	input.ClearBindings();

	auto bg1 = std::make_unique<dae::GameObject>();
	bg1->AddComponent<dae::TextureComponent>("background.png", bg1.get());
	bg1->AddComponent<dae::BackgroundScrollComponent>(bg1.get(), 80.f, 480.f, 0.f);
	bg1->SetPosition(0, 0);

	scene.Add(std::move(bg1));

	auto bg2 = std::make_unique<dae::GameObject>();
	bg2->AddComponent<dae::TextureComponent>("background.png", bg2.get());
	bg2->AddComponent<dae::BackgroundScrollComponent>(bg2.get(), 80.f, 480.f, -480.f);
	bg2->SetPosition(0, -480);

	scene.Add(std::move(bg2));

	auto menuManager = std::make_unique<dae::GameObject>();
	menuManager->AddComponent<MenuManager>(menuManager.get());

	menuManager->AddObserver(menuManager->GetComponent<MenuManager>());

	auto titleText = std::make_unique<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	titleText->AddComponent<dae::TextComponent>("GALAGA", font, titleText.get());
	titleText->SetPosition(250, 100);
	scene.Add(std::move(titleText));

	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

	std::vector<std::string> menuOptions = {"1 PLAYER","2 PLAYER","PvP", "High Scores","EXIT"};

	for (int i{}; i < menuOptions.size(); ++i)
	{
		auto menuItem = std::make_unique<dae::GameObject>();
		menuItem->AddComponent<MenuItemComponent>(menuItem.get(), static_cast<int>(i), menuOptions[i], font);
		menuItem->SetPosition(250, static_cast<float>(200 + i * 40));

		menuManager->AddObserver(menuItem->GetComponent<MenuItemComponent>());
		scene.Add(std::move(menuItem));
	}

	input.BindCommand(SDL_SCANCODE_UP, std::make_unique<UpUiCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_DOWN, std::make_unique<DownUiCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_W, std::make_unique<UpUiCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_S, std::make_unique<DownUiCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_RETURN, std::make_unique<ConfirmUiCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_F2, std::make_unique<MuteCommand>(menuManager.get()));
	input.BindCommand(SDL_SCANCODE_KP_ENTER, std::make_unique<ConfirmUiCommand>(menuManager.get()));
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_UP, std::make_unique<UpUiCommand>(menuManager.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_DOWN, std::make_unique<DownUiCommand>(menuManager.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<ConfirmUiCommand>(menuManager.get()), 0);

	scene.Add(std::move(menuManager));

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

	ServiceLocator::getSoundSystem().play(11, 0.8f);
}

void loadSinglePlayer()
{
	ServiceLocator::getSoundSystem().stopAllSounds();

	dae::SceneManager::GetInstance().RemoveAllScenes();

	auto& scene = dae::SceneManager::GetInstance().CreateScene("SinglePlayer");
	dae::SceneManager::GetInstance().SetActiveScene("SinglePlayer");

	auto& input = dae::InputManager::GetInstance();

	input.ClearBindings();

	auto fo = std::make_unique<dae::GameObject>();
	fo->AddComponent<GalagaGameManager>(fo.get());

	fo->AddObserver(fo->GetComponent<GalagaGameManager>());

	auto bg1 = std::make_unique<dae::GameObject>();
	bg1->AddComponent<dae::TextureComponent>("background.png", bg1.get());
	bg1->AddComponent<dae::BackgroundScrollComponent>(bg1.get(), 80.f, 480.f, 0.f);
	bg1->SetPosition(0, 0);

	fo->AddObserver(bg1.get()->GetComponent<Observer>());

	scene.Add(std::move(bg1));

	auto bg2 = std::make_unique<dae::GameObject>();
	bg2->AddComponent<dae::TextureComponent>("background.png", bg2.get());
	bg2->AddComponent<dae::BackgroundScrollComponent>(bg2.get(), 80.f, 480.f, -480.f);
	bg2->SetPosition(0, -480);

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
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_LEFT, std::make_unique<MoveLeftCommand>(go.get()), 0);
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, std::make_unique<MoveRightCommand>(go.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, std::make_unique<FireCommand>(go.get()), 0);

	input.BindCommand(SDL_SCANCODE_ESCAPE, std::make_unique<PauseCommand>(fo.get()));
	input.BindCommand(SDL_CONTROLLER_BUTTON_START, std::make_unique<PauseCommand>(fo.get()), 0);
	input.BindCommand(SDL_SCANCODE_UP, std::make_unique<UpUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_DOWN, std::make_unique<DownUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_W, std::make_unique<UpUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_S, std::make_unique<DownUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_RETURN, std::make_unique<ConfirmUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_KP_ENTER, std::make_unique<ConfirmUiCommand>(fo.get()));
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_UP, std::make_unique<UpUiCommand>(fo.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_DOWN, std::make_unique<DownUiCommand>(fo.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<ConfirmUiCommand>(fo.get()), 0);
	input.BindCommand(SDL_SCANCODE_F2, std::make_unique<MuteCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_F1, std::make_unique<SkipLevelCommand>(fo.get()));

	go->AddObserver(fo.get()->GetComponent<Observer>());
	fo->AddObserver(go.get()->GetComponent<Observer>());

	scene.Add(std::move(go));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
	std::vector<std::string> pauseOptions = { "RESUME", "EXIT TO MAIN MENU" };

	for (int i{}; i < pauseOptions.size(); ++i)
	{
		auto pauseMenuItem = std::make_unique<dae::GameObject>();

		pauseMenuItem->AddComponent<MenuItemComponent>(pauseMenuItem.get(),static_cast<int>(i),pauseOptions[i],font);

		pauseMenuItem->SetPosition(250, static_cast<float>(240 + i * 40));
		pauseMenuItem->GetComponent<MenuItemComponent>()->HideToggle();
		fo->AddObserver(pauseMenuItem->GetComponent<MenuItemComponent>());

		scene.Add(std::move(pauseMenuItem));
	}

	scene.Add(std::move(fo));
}

void loadTwoPlayer()
{
	ServiceLocator::getSoundSystem().stopAllSounds();

	dae::SceneManager::GetInstance().RemoveAllScenes();

	auto& scene = dae::SceneManager::GetInstance().CreateScene("TwoPlayer");
	dae::SceneManager::GetInstance().SetActiveScene("TwoPlayer");

	auto& input = dae::InputManager::GetInstance();
	input.ClearBindings();

	auto fo = std::make_unique<dae::GameObject>();
	fo->AddComponent<GalagaGameManager>(fo.get());
	fo->AddObserver(fo->GetComponent<GalagaGameManager>());

	auto bg1 = std::make_unique<dae::GameObject>();
	bg1->AddComponent<dae::TextureComponent>("background.png", bg1.get());
	bg1->AddComponent<dae::BackgroundScrollComponent>(bg1.get(), 80.f, 480.f, 0.f);
	bg1->SetPosition(0, 0);
	fo->AddObserver(bg1.get()->GetComponent<Observer>());
	scene.Add(std::move(bg1));

	auto bg2 = std::make_unique<dae::GameObject>();
	bg2->AddComponent<dae::TextureComponent>("background.png", bg2.get());
	bg2->AddComponent<dae::BackgroundScrollComponent>(bg2.get(), 80.f, 480.f, -480.f);
	bg2->SetPosition(0, -480);
	fo->AddObserver(bg2.get()->GetComponent<Observer>());
	scene.Add(std::move(bg2));

	auto go = std::make_unique<dae::GameObject>();
	go->SetPosition(250, 400);
	go->AddComponent<dae::PlayerComponent>(go.get(), "galaga.png", "Bullet.png", 10);
	go->AddComponent<dae::BoundsComponent>(go.get(), 640.f, 480.f);

	input.BindContinuousCommand(SDL_SCANCODE_A, std::make_unique<MoveLeftCommand>(go.get()));
	input.BindContinuousCommand(SDL_SCANCODE_D, std::make_unique<MoveRightCommand>(go.get()));
	input.BindCommand(SDL_SCANCODE_SPACE, std::make_unique<FireCommand>(go.get()));
	 
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_LEFT, std::make_unique<MoveLeftCommand>(go.get()), 0);
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, std::make_unique<MoveRightCommand>(go.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<FireCommand>(go.get()), 0);

	go->AddObserver(fo.get()->GetComponent<Observer>());
	fo->AddObserver(go.get()->GetComponent<Observer>());
	scene.Add(std::move(go));

	auto go2 = std::make_unique<dae::GameObject>();
	go2->SetPosition(350, 400);
	go2->AddComponent<dae::PlayerComponent>(go2.get(), "galaga.png", "Bullet.png", 10);
	go2->AddComponent<dae::BoundsComponent>(go2.get(), 640.f, 480.f);

	input.BindContinuousCommand(SDL_SCANCODE_LEFT, std::make_unique<MoveLeftCommand>(go2.get()));
	input.BindContinuousCommand(SDL_SCANCODE_RIGHT, std::make_unique<MoveRightCommand>(go2.get()));
	input.BindCommand(SDL_SCANCODE_RCTRL, std::make_unique<FireCommand>(go2.get()));

	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_LEFT, std::make_unique<MoveLeftCommand>(go2.get()), 1);
	input.BindContinuousCommand(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, std::make_unique<MoveRightCommand>(go2.get()), 1);
	input.BindCommand(SDL_CONTROLLER_BUTTON_A, std::make_unique<FireCommand>(go2.get()), 1);

	go2->AddObserver(fo.get()->GetComponent<Observer>());
	fo->AddObserver(go2.get()->GetComponent<Observer>());
	scene.Add(std::move(go2));

	input.BindCommand(SDL_SCANCODE_ESCAPE, std::make_unique<PauseCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_UP, std::make_unique<UpUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_DOWN, std::make_unique<DownUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_W, std::make_unique<UpUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_S, std::make_unique<DownUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_RETURN, std::make_unique<ConfirmUiCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_KP_ENTER, std::make_unique<ConfirmUiCommand>(fo.get()));

	input.BindCommand(SDL_CONTROLLER_BUTTON_START, std::make_unique<PauseCommand>(fo.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_UP, std::make_unique<UpUiCommand>(fo.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_DOWN, std::make_unique<DownUiCommand>(fo.get()), 0);
	input.BindCommand(SDL_CONTROLLER_BUTTON_B, std::make_unique<ConfirmUiCommand>(fo.get()), 0);

	input.BindCommand(SDL_CONTROLLER_BUTTON_START, std::make_unique<PauseCommand>(fo.get()), 1);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_UP, std::make_unique<UpUiCommand>(fo.get()), 1);
	input.BindCommand(SDL_CONTROLLER_BUTTON_DPAD_DOWN, std::make_unique<DownUiCommand>(fo.get()), 1);
	input.BindCommand(SDL_CONTROLLER_BUTTON_B, std::make_unique<ConfirmUiCommand>(fo.get()), 1);

	input.BindCommand(SDL_SCANCODE_F2, std::make_unique<MuteCommand>(fo.get()));
	input.BindCommand(SDL_SCANCODE_F1, std::make_unique<SkipLevelCommand>(fo.get()));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
	std::vector<std::string> pauseOptions = { "RESUME", "EXIT TO MAIN MENU" };

	for (int i{}; i < pauseOptions.size(); ++i)
	{
		auto pauseMenuItem = std::make_unique<dae::GameObject>();
		pauseMenuItem->AddComponent<MenuItemComponent>(pauseMenuItem.get(), static_cast<int>(i), pauseOptions[i], font);
		pauseMenuItem->SetPosition(250, static_cast<float>(240 + i * 40));
		pauseMenuItem->GetComponent<MenuItemComponent>()->HideToggle();
		fo->AddObserver(pauseMenuItem->GetComponent<MenuItemComponent>());
		scene.Add(std::move(pauseMenuItem));
	}

	scene.Add(std::move(fo));
}
void loadPvP()
{
	ServiceLocator::getSoundSystem().stopAllSounds();

	dae::SceneManager::GetInstance().RemoveAllScenes();

	auto& scene = dae::SceneManager::GetInstance().CreateScene("PvP");
	scene.RemoveAll();

	auto& input = dae::InputManager::GetInstance();

	input.ClearBindings();
}

void loadHighScores()
{
	ServiceLocator::getSoundSystem().stopAllSounds();

	dae::SceneManager::GetInstance().RemoveAllScenes();

	auto& scene = dae::SceneManager::GetInstance().CreateScene("HighScores");
	dae::SceneManager::GetInstance().SetActiveScene("HighScores");

	auto& input = dae::InputManager::GetInstance();
	input.ClearBindings();

	auto bg1 = std::make_unique<dae::GameObject>();
	bg1->AddComponent<dae::TextureComponent>("background.png", bg1.get());
	bg1->AddComponent<dae::BackgroundScrollComponent>(bg1.get(), 40.f, 480.f, 0.f);
	bg1->SetPosition(0, 0);
	scene.Add(std::move(bg1));

	auto bg2 = std::make_unique<dae::GameObject>();
	bg2->AddComponent<dae::TextureComponent>("background.png", bg2.get());
	bg2->AddComponent<dae::BackgroundScrollComponent>(bg2.get(), 40.f, 480.f, -480.f);
	bg2->SetPosition(0, -480);
	scene.Add(std::move(bg2));

	auto& highScoreManager = HighScoreManager::GetInstance();
	auto scores = highScoreManager.GetTopScores();

	auto titleText = std::make_unique<dae::GameObject>();
	auto titleFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	titleText->AddComponent<dae::TextComponent>("HIGH SCORES", titleFont, titleText.get());
	titleText->SetPosition(200, 80);
	scene.Add(std::move(titleText));

	auto scoreFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);

	if (!scores.empty())
	{
		for (int i{}; i < scores.size(); ++i)
		{
			auto rankNameText = std::make_unique<dae::GameObject>();
			std::string rankName = std::to_string(i + 1) + ". " + scores[i].playerName;
			rankNameText->AddComponent<dae::TextComponent>(rankName, scoreFont, rankNameText.get());
			rankNameText->SetPosition(150, static_cast<float>(150 + i * 35));
			scene.Add(std::move(rankNameText));

			auto scoreText = std::make_unique<dae::GameObject>();
			std::string scoreStr = std::to_string(scores[i].score);
			scoreText->AddComponent<dae::TextComponent>(scoreStr, scoreFont, scoreText.get());
			scoreText->SetPosition(400, static_cast<float>(150 + i * 35));
			scene.Add(std::move(scoreText));
		}
	}
	else
	{
		auto noScoresText = std::make_unique<dae::GameObject>();
		noScoresText->AddComponent<dae::TextComponent>("No high scores yet", scoreFont, noScoresText.get());
		noScoresText->SetPosition(200, 200);
		scene.Add(std::move(noScoresText));

		auto playText = std::make_unique<dae::GameObject>();
		playText->AddComponent<dae::TextComponent>("Play the game to set a high score", scoreFont, playText.get());
		playText->SetPosition(150, 240);
		scene.Add(std::move(playText));
	}

	auto instructionText = std::make_unique<dae::GameObject>();
	auto instructionFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	instructionText->AddComponent<dae::TextComponent>("Press ESC or B to return to main menu", instructionFont, instructionText.get());
	instructionText->SetPosition(150, 420);
	scene.Add(std::move(instructionText));

	auto inputHandler = std::make_unique<dae::GameObject>();
	input.BindCommand(SDL_SCANCODE_ESCAPE, std::make_unique<ReturnToMenuCommand>(inputHandler.get()));
	input.BindCommand(SDL_SCANCODE_F2, std::make_unique<MuteCommand>(inputHandler.get()));
	input.BindCommand(SDL_CONTROLLER_BUTTON_B, std::make_unique<ReturnToMenuCommand>(inputHandler.get()), 0);
	scene.Add(std::move(inputHandler));

	ServiceLocator::getSoundSystem().play(6, 0.8f);
}

int main(int, char* [])
{
	dae::Minigin engine("../Data/");
	engine.Run(loadMainMenu);

	return 0;
}