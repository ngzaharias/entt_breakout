#include "Game.hpp"

#include "Map.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Screen.hpp"
#include "VectorHelper.hpp"

#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include "Systems/Scene.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Game* Game::s_Instance = nullptr;

Game::Game()
{
	s_Instance = this;
	m_Map = new Map();
}

Game::~Game()
{
	delete m_Map;

	s_Instance = nullptr;
}

void Game::Initialise(entt::registry& registry)
{
	m_EnttDebugger.Initialise(registry);

	m_Registry = &registry;

	entt::sink sink{ m_Scene.m_OnCollisionSignal };
	sink.connect<&Game::OnCollision>(this);

	m_Map->Load(registry);
}

void Game::Destroy(entt::registry& registry)
{
	m_Map->Unload(registry);
}

void Game::Update(entt::registry& registry, const sf::Time& time)
{
	sf::Time timeScaled = time;
	if (m_IsPaused == true)
	{
		timeScaled = sf::Time::Zero;
	}

	//#todo: fixed update
	m_Scene.Update(registry, timeScaled);
	m_EnttDebugger.Update(registry, time);

	m_Map->Update(registry, timeScaled);
}

void Game::Draw(entt::registry& registry, sf::RenderWindow* window)
{
	m_Map->Draw(window);

	m_Renderer.Update(registry, window);
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

Map& Game::GetMap() const
{
	return *m_Map;
}

void Game::OnCollision(const entt::entity& entityA, const entt::entity& entityB)
{
	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::Brick>(entityB))
	{
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
		const sf::Vector2f directionToBall = VectorHelper::Normalize(ballPosition - paddlePosition);

		const float dot = VectorHelper::Dot(directionToBall, sf::Vector2f(0.0f, -1.0f));
		const float influenceX = Math::Clamp(directionToBall.x, -0.7f, 0.7f);
		const float influenceY = -Math::Clamp(dot + 0.2f, 0.0f, 1.0f);

		float magnitude = VectorHelper::Magnitude(ballVelocity.m_Velocity);
		magnitude = Math::Min<float>(magnitude + 100.0f, m_Map->m_BallSettings.velocityMax);

		sf::Vector2f directionOld = ballVelocity.m_Velocity / magnitude;
		sf::Vector2f directionNew = sf::Vector2f(influenceX, influenceY);
		directionNew = VectorHelper::Normalize(directionOld + directionNew);
		ballVelocity.m_Velocity = directionNew * magnitude;
	}

	if (m_Registry->try_get<tag::Ball>(entityA) && m_Registry->try_get<tag::RespawnZone>(entityB))
	{
		auto& ballTransform = m_Registry->get<core::Transform>(entityA);
		auto& ballVelocity = m_Registry->get<physics::Velocity>(entityA);

		m_Map->UpdateLives(-1);

		sf::Vector2f direction = sf::Vector2f(Random::Range(-0.5f, 0.5f), Random::Range(-1.0f, -0.5f));
		direction = VectorHelper::Normalize(direction);

		ballTransform.m_Translate = m_Map->m_BallSettings.position;
		ballVelocity.m_Velocity = direction * m_Map->m_BallSettings.velocityMin;
	}
}

Game& Game::Instance()
{
	return *s_Instance;
}

