#include "Game.hpp"

#include "Math.hpp"
#include "Random.hpp"
#include "Screen.hpp"
#include "VectorHelpers.hpp"

#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Game* Game::s_Instance = nullptr;

Game::Game()
{
	s_Instance = this;
}

Game::~Game()
{
	s_Instance = nullptr;
}

void Game::Initialise(entt::registry& registry)
{
	m_EnttDebugger.Initialise(registry);

	m_Registry = &registry;

	entt::sink sink{ m_PhysicsSystem.m_OnCollisionSignal };
	sink.connect<&Game::OnCollision>(this);

	m_LevelSystem.Load(registry, core::s_LevelPath);
	m_SoundSystem.Initialize();
}

void Game::Destroy(entt::registry& registry)
{
	m_LevelSystem.Unload(registry);
}

void Game::Update(entt::registry& registry, const sf::Time& time)
{
	sf::Time timeScaled = time;
	//if (m_IsPaused == true)
	//{
	//	timeScaled = sf::Time::Zero;
	//}

	// #todo: fixed update
	m_LevelSystem.Update(registry, timeScaled);
	m_PhysicsSystem.Update(registry, timeScaled);
	m_SoundSystem.Update(registry, time);

	m_EnttDebugger.Update(registry, time);
}

void Game::Draw(entt::registry& registry, sf::RenderWindow* window)
{
	m_LevelSystem.Draw(window);

	m_RenderSystem.Update(registry, window);
	m_EnttDebugger.Render(registry);
}

void Game::Pause()
{
	m_IsPaused = true;
}

void Game::Unpause()
{
	m_IsPaused = false;
}

void Game::OnCollision(const entt::entity& entityA, const entt::entity& entityB)
{
	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::Brick>(entityB))
	{
		m_LevelSystem.UpdateScore(1);
		m_SoundSystem.PlaySound(audio::Name::Impact);
		m_Registry->destroy(entityB);
		return;
	}

	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::Paddle>(entityB))
	{
		auto& ballTransform = m_Registry->get<core::Transform>(entityA);
		auto& ballVelocity = m_Registry->get<physics::Velocity>(entityA);
		auto& paddleTransform = m_Registry->get<core::Transform>(entityB);

		const sf::Vector2f ballPosition = ballTransform.m_Translate;
		const sf::Vector2f paddlePosition = paddleTransform.m_Translate;
		const sf::Vector2f directionToBall = Normalized(ballPosition - paddlePosition);

		const float dot = Dot(directionToBall, sf::Vector2f(0.0f, -1.0f));
		const float influenceX = Math::Clamp(directionToBall.x, -0.7f, 0.7f);
		const float influenceY = -Math::Clamp(dot + 0.2f, 0.0f, 1.0f);

		float length = Length(ballVelocity.m_Velocity);
		length = Math::Min<float>(length + 100.0f, m_LevelSystem.m_BallSettings.velocityMax);

		sf::Vector2f directionOld = ballVelocity.m_Velocity / length;
		sf::Vector2f directionNew = sf::Vector2f(influenceX, influenceY);
		directionNew = Normalized(directionOld + directionNew);
		ballVelocity.m_Velocity = directionNew * length;

		m_SoundSystem.PlaySound(audio::Name::Impact);
	}

	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::RespawnZone>(entityB))
	{
		auto& ballTransform = m_Registry->get<core::Transform>(entityA);
		auto& ballVelocity = m_Registry->get<physics::Velocity>(entityA);

		m_LevelSystem.UpdateLives(-1);

		sf::Vector2f direction = sf::Vector2f(random::Range(-0.5f, 0.5f), random::Range(-1.0f, -0.5f));
		direction = Normalized(direction);

		ballTransform.m_Translate = m_LevelSystem.m_BallSettings.position;
		ballVelocity.m_Velocity = direction * m_LevelSystem.m_BallSettings.velocityMin;
	}

	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::Wall>(entityB))
	{
		m_SoundSystem.PlaySound(audio::Name::Impact);
	}
}

Game& Game::Instance()
{
	return *s_Instance;
}

