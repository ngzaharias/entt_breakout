#include "Game.hpp"

#include "Map.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Screen.hpp"
#include "VectorHelper.hpp"

#include "Components/Collider.hpp"
#include "Components/Camera.hpp"
#include "Components/Level.hpp"
#include "Components/Name.hpp"
#include "Components/Rigidbody.hpp"
#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include "Systems/Scene.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Game* Game::s_Instance = nullptr;
entt::editor Game::s_Editor = entt::editor();
entt::entity Game::s_Entity = entt::null;

namespace
{
	bool s_bIsEditorEnabled = false;
}

namespace MM 
{
	template <>
	void ComponentEditorWidget<core::Level>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<core::Level>(entity);
		ImGui::Text("%s", component.m_Name.c_str());
	}

	template <>
	void ComponentEditorWidget<core::Transform>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<core::Transform>(entity);
		ImGui::DragFloat2("Translate", &component.m_Translate.x);
		ImGui::DragFloat("Rotate", &component.m_Rotate);
		ImGui::DragFloat2("Scale", &component.m_Scale.x);
	}

	template <>
	void ComponentEditorWidget<debug::Name>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<debug::Name>(entity);
		ImGui::Text("%s", component.m_Name.c_str());
	}

	template <>
	void ComponentEditorWidget<physics::Collider>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<physics::Collider>(entity);
		ImGui::DragFloat2("", &component.m_Extents.x);
	}

	template <>
	void ComponentEditorWidget<physics::Velocity>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<physics::Velocity>(entity);
		ImGui::DragFloat2("", &component.m_Velocity.x);
	}

	template <>
	void ComponentEditorWidget<sf::RectangleShape>(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<sf::RectangleShape>(entity);
		auto fillColor = component.getFillColor();
		auto size = component.getSize();

		float fillColor_v[4] = { fillColor.r / 255.f, fillColor.g / 255.f, fillColor.b / 255.f, fillColor.a / 255.f };
		float size_v[2] = { size.x, size.y };
		ImGui::ColorEdit4("Fill Color", fillColor_v, ImGuiColorEditFlags_Uint8);
		ImGui::DragFloat2("Size", size_v);

		fillColor.r = static_cast<sf::Uint8>(fillColor_v[0] * 255.f);
		fillColor.g = static_cast<sf::Uint8>(fillColor_v[1] * 255.f);
		fillColor.b = static_cast<sf::Uint8>(fillColor_v[2] * 255.f);
		fillColor.a = static_cast<sf::Uint8>(fillColor_v[3] * 255.f);
		size.x = size_v[0];
		size.y = size_v[1];

		component.setFillColor(fillColor);
		component.setSize(size);
	}
}

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
	m_Registry = &registry;
	s_Entity = registry.create();

	s_Editor.registerComponent<core::Camera>("core::Camera");
	s_Editor.registerComponent<core::Level>("core::Level");
	s_Editor.registerComponent<core::Transform>("core::Transform");

	s_Editor.registerComponent<debug::Name>("debug::Name");

	s_Editor.registerComponent<tag::Ball>("tag::Ball");
	s_Editor.registerComponent<tag::Brick>("tag::Brick");
	s_Editor.registerComponent<tag::Paddle>("tag::Paddle");
	s_Editor.registerComponent<tag::RespawnZone>("tag::RespawnZone");
	s_Editor.registerComponent<tag::Wall>("tag::Wall");

	s_Editor.registerComponent<physics::Collider>("physics::Collider");
	s_Editor.registerComponent<physics::Rigidbody>("physics::Rigidbody");
	s_Editor.registerComponent<physics::Velocity>("physics::Velocity");

	s_Editor.registerComponent<sf::RectangleShape>("sf::RectangleShape");

	{
		entt::entity entity = registry.create();
		auto& camera = registry.emplace<core::Camera>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		camera.m_View = sf::View(sf::FloatRect(0.f, 0.f, Screen::width, Screen::height));
	}

	entt::sink sink{ m_Scene.m_OnCollisionSignal };
	sink.connect<&Game::OnCollision>(this);

	m_Map->Load(registry);
}

void Game::Destroy(entt::registry& registry)
{
	m_Map->Unload(registry);

	s_Editor = entt::editor();

	registry.destroy(s_Entity);
}

void Game::Update(entt::registry& registry, const sf::Time& time)
{
	sf::Time timeScaled = time;
	if (m_IsPaused == true)
	{
		timeScaled = sf::Time::Zero;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
		s_bIsEditorEnabled = !s_bIsEditorEnabled;

	//#todo: fixed update
	m_Scene.Update(registry, timeScaled);

	m_Map->Update(registry, timeScaled);
}

void Game::Draw(entt::registry& registry, sf::RenderWindow* window)
{
	m_Map->Draw(window);

	m_Renderer.Update(registry, window);

	if (s_bIsEditorEnabled)
	{
		s_Editor.renderSimpleCombo(registry, s_Entity);
	}
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

