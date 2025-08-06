    #pragma once

    #include "Component.h"
    #include "GameObject.h"
    #include "BaseAIController.h"
    #include "TractorBeamComponent.h"
    #include "EnemyPlayerAIComponent.h"

    class BossAIControllerComponent : public dae::Component, public BaseAIController
    {
    public:
        BossAIControllerComponent(dae::GameObject* owner);
        virtual ~BossAIControllerComponent() = default;

        void Update(const float deltaTime) override;
        void Render() const override;

        void StartTractorBeam();
        void ShootTractorBeam();
        bool IsTractorBeamActive() const { return m_TractorBeamActive; }

        void ShootFighter();

        virtual void SetAttack(bool SetAttack) override;

        void CreateGalagaEnemyPlayer();

    private:
        bool m_TractorBeamActive{};
        bool m_IsTractorBeamRun{};
        bool m_DoOnce{};
        float m_TractorBeamTimer{};
        float m_TractorBeamDuration{3.f};
        float m_formationWobble{};
        dae::GameObject* m_TractorBeamTarget{ nullptr };
		EnemyPlayerAIComponent* m_EnemyPlayerComponent{ nullptr };

        void UpdateTractorBeam(float deltaTime);

        void OnUpdateFormationBehavior(float deltaTime) override;
        void OnGenerateDivePath(std::vector<glm::vec3>& path) override;
        void Shoot() override;
    };

