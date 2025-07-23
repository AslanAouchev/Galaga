#include "NameInputComponent.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "HighScoreManager.h"
#include "SceneManager.h"
#include "Scene.h"

NameInputComponent::NameInputComponent(dae::GameObject* owner, int score, int shots, int enemies, float accuracy)
    : Component(owner), m_Score(score), m_ShotsTotal(shots), m_EnemiesKilled(enemies), m_Accuracy(accuracy)
{
    m_Font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
    CreateTextComponents();
    UpdateNameDisplay();
    
}

void NameInputComponent::Update(float deltaTime)
{
    if (m_IsActive)
    {
        UpdateVisualFeedback(deltaTime);
    }
}

void NameInputComponent::OnNotify(const EventData& event)
{
    if (event.eventType == "EndGame")
    {
        SetActive(true);
    }

    if (!m_IsActive) return;

    if (event.eventType == "NameUp")
    {
        char currentChar{ m_CurrentName[m_NamePosition] };
        ++currentChar;
        if (currentChar > 'Z') currentChar = 'A';
        m_CurrentName[m_NamePosition] = currentChar;
        UpdateNameDisplay();
    }
    else if (event.eventType == "NameDown")
    {
        char currentChar{ m_CurrentName[m_NamePosition] };
        --currentChar;
        if (currentChar < 'A') currentChar = 'Z';
        m_CurrentName[m_NamePosition] = currentChar;
        UpdateNameDisplay();
    }
    else if (event.eventType == "NameLeftComponent")
    {
        --m_NamePosition;
        if (m_NamePosition < 0) m_NamePosition = 2;
        UpdateNameDisplay();
    }
    else if (event.eventType == "NameRightComponent")
    {
        ++m_NamePosition;
        if (m_NamePosition > 2) m_NamePosition = 0;
        UpdateNameDisplay();
    }
    else if (event.eventType == "NameInputComplete")
    {
        SetActive(false);
        auto& highScoreManager = HighScoreManager::GetInstance();
        highScoreManager.AddHighScore(m_CurrentName, m_Score);
		std::cout << "New high score added: " << m_CurrentName << " with score: " << m_Score << std::endl;
        extern void loadHighScores();
        loadHighScores();
    }
}

void NameInputComponent::CreateTextComponents()
{
    auto& scene = dae::SceneManager::GetInstance().GetActiveScene();

    auto titleFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    auto regularFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
    auto statsFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
    auto instructionFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);

    auto titleObject = std::make_unique<dae::GameObject>();
    titleObject->SetPosition(200, 120);
    titleObject->AddComponent<dae::TextComponent>("NEW HIGH SCORE!", titleFont, titleObject.get());
    m_TitleObject = titleObject.get();
    scene.Add(std::move(titleObject));

    auto scoreObject = std::make_unique<dae::GameObject>();
    scoreObject->SetPosition(250, 180);
    std::string scoreText{ "SCORE: " + std::to_string(m_Score) };
    scoreObject->AddComponent<dae::TextComponent>(scoreText, regularFont, scoreObject.get());
    m_ScoreObject = scoreObject.get();
    scene.Add(std::move(scoreObject));

    auto statsObject = std::make_unique<dae::GameObject>();
    statsObject->SetPosition(120, 220);
    std::ostringstream statsStream;
    statsStream << "SHOTS: " << m_ShotsTotal << "  HITS: " << m_EnemiesKilled << "  ACCURACY: " << std::fixed << std::setprecision(1) << m_Accuracy << "%";
    statsObject->AddComponent<dae::TextComponent>(statsStream.str(), statsFont, statsObject.get());
    m_StatsObject = statsObject.get();
    scene.Add(std::move(statsObject));

    auto nameObject = std::make_unique<dae::GameObject>();
    nameObject->SetPosition(250, 280);
    nameObject->AddComponent<dae::TextComponent>("NAME: AAA", regularFont, nameObject.get());
    m_NameObject = nameObject.get();
    scene.Add(std::move(nameObject));

    HideTextObjects();
}

void NameInputComponent::ShowTextObjects()
{
    if (m_TitleObject) m_TitleObject->SetPosition(200, 120);
    if (m_ScoreObject) m_ScoreObject->SetPosition(250, 180);
    if (m_StatsObject) m_StatsObject->SetPosition(120, 220);
    if (m_NameObject) m_NameObject->SetPosition(250, 280);
    if (m_InstructionObject) m_InstructionObject->SetPosition(100, 350);
}

void NameInputComponent::HideTextObjects()
{
    const float offScreenY = -10000.0f;

    if (m_TitleObject) m_TitleObject->SetPosition(200, offScreenY);
    if (m_ScoreObject) m_ScoreObject->SetPosition(250, offScreenY);
    if (m_StatsObject) m_StatsObject->SetPosition(120, offScreenY);
    if (m_NameObject) m_NameObject->SetPosition(250, offScreenY);
    if (m_InstructionObject) m_InstructionObject->SetPosition(100, offScreenY);
}

void NameInputComponent::UpdateNameDisplay()
{
    if (!m_NameObject) return;

    auto textComponent = m_NameObject->GetComponent<dae::TextComponent>();
    if (!textComponent) return;

    std::string displayName{ m_CurrentName };

    if (m_ShowBlink)
    {
        std::string nameWithCursor = "NAME: ";
        for (int i{}; i < 3; ++i)
        {
            if (i == m_NamePosition)
            {
                nameWithCursor += "[" + std::string(1, displayName[i]) + "]";
            }
            else
            {
                nameWithCursor += " " + std::string(1, displayName[i]) + " ";
            }
        }
        textComponent->SetText(nameWithCursor);
    }
    else
    {
        textComponent->SetText("NAME:  " + displayName);
    }
}

void NameInputComponent::SetActive(bool active)
{
    m_IsActive = active;

    if (active)
    {
        ShowTextObjects();
    }
    else
    {
        HideTextObjects();
    }
}

void NameInputComponent::UpdateVisualFeedback(float deltaTime)
{
    m_BlinkTimer += deltaTime;
    if (m_BlinkTimer >= m_BlinkInterval)
    {
        m_ShowBlink = !m_ShowBlink;
        m_BlinkTimer = 0.0f;
        UpdateNameDisplay();
    }
}