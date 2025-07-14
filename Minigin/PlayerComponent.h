#pragma once
#include <memory>
#include "GameObject.h"
#include "Component.h"
#include "TextureComponent.h"

namespace dae
{
	enum class GameObjectTag
	{
		Player,
		Bee,
		Boss,
		Butterfly,
		EnemyPlayer
	};

	enum class BulletTag
	{
		PlayerBullet,
		EnemyBullet
	};

	class PlayerComponent final : public Component, public Observer
	{
	public:
		void Update(const float deltaTime) override;
		void Render() const override;

		PlayerComponent(GameObject* pOwner, const std::string& textureFileName,const std::string& bulletTextureFileName, int health, float bulletSpeed = 400.f);

		PlayerComponent(const PlayerComponent& other) = delete;
		PlayerComponent(PlayerComponent&& other) = delete;
		PlayerComponent& operator=(const PlayerComponent& other) = delete;
		PlayerComponent& operator=(PlayerComponent&& other) = delete;

		void TakeDamage(int amount);
		bool IsDead() const { return m_IsDead; }
		bool Fire();
		glm::vec2 GetTextureSize() const;

		GameObjectTag GetTag() const { return m_Tag; }
		void SetTag(GameObjectTag tag) { m_Tag = tag; }
		BulletTag GetBulletTag() const { return m_BulletTag; }
		void SetBulletTag(BulletTag bulletTag) { m_BulletTag = bulletTag; }
		int GetScore() const { return m_ScoreValue; }
		void SetScore(int score) { m_ScoreValue = score; }

		void OnNotify(const EventData& event) override;

	private:
		void CheckCollisions();
		void UpdateKilledPause(float deltaTime);

		int m_Health{};
		int m_MaxHealth{};
		bool m_IsDead{};
		GameObjectTag m_Tag{ GameObjectTag::Player };
		BulletTag m_BulletTag{ BulletTag::PlayerBullet };

		std::unique_ptr<dae::TextureComponent> m_pTexture{nullptr};

		std::string m_BulletString{""};
		float m_FireCooldown{ 0.0f };
		const float m_MaxFireCooldown{ 0.2f };
		const float m_BulletSpeed{ 400.0f };
		int m_ScoreValue{ 0 };
		bool m_Paused{ false };

		bool m_KilledPaused{ false };
		float m_KilledPauseTimer{ 0.0f };
	};
}