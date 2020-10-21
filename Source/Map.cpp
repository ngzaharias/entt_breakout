#include "Map.hpp"

#include "Game.hpp"
#include "JsonHelper.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Screen.hpp"
#include "VectorHelper.hpp"

#include "Components/Camera.hpp"
#include "Components/Collider.hpp"
#include "Components/Level.hpp"
#include "Components/Name.hpp"
#include "Components/Rigidbody.hpp"
#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include <entt/entt.hpp>
#include <rapidjson/document.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Map::Map()
{
}

Map::~Map()
{
}

void Map::Update(entt::registry& registry, const sf::Time& time)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		Unload(registry);
		Load(registry);
		Game::Instance().Unpause();
	}

	{
		const sf::RenderWindow* window = Screen::GetWindow();
		const sf::Vector2i screenPosition = sf::Mouse::getPosition(*window);
		const sf::Vector2f worldPosition = window->mapPixelToCoords(screenPosition);

		const auto view = registry.view<core::Transform, tag::Paddle>();
		for (const entt::entity& entity : view)
		{
			auto& transform = registry.get<core::Transform>(entity);

			const float target = Math::Clamp(worldPosition.x, Screen::width * -0.5f, Screen::width * 0.5f);
			transform.m_Translate.x += (target - transform.m_Translate.x) * 100.0f * time.asSeconds();
		}
	}
}

void Map::Draw(sf::RenderWindow* window)
{
	window->draw(m_infoText);
	window->draw(m_livesText);
	window->draw(m_scoreText);
}

void Map::Load(entt::registry& registry)
{
	std::string levelPath = "Assets/Settings/Maps/BasicMap.json";
	std::string levelName = "BasicMap";

	// level
	{
		rapidjson::Document document;
		if (JsonHelper::LoadDocument(levelPath.c_str(), document) == true)
		{
			LoadSettings(document);
		}

		m_font.loadFromFile(m_MapSettings.fontpath);

		m_infoText.setFont(m_font);
		m_infoText.setCharacterSize(20);
		m_infoText.setPosition(0.f - 80.0f, Screen::height * 0.5f - 40.0f);
		m_infoText.setString("ESC TO RELOAD");
		m_infoText.setStyle(sf::Text::Bold);

		m_livesText.setFont(m_font);
		m_livesText.setPosition(Screen::width * -0.5f + 40.0f, Screen::height * 0.5f - 40.0f);
		m_livesText.setStyle(sf::Text::Bold);

		m_scoreText.setFont(m_font);
		m_scoreText.setPosition(Screen::width * 0.5f - 60.0f, Screen::height * 0.5f - 40.0f);
		m_scoreText.setStyle(sf::Text::Bold);
	}

	// balls
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& rigidbody = registry.emplace<physics::Rigidbody>(entity);
		auto& tag = registry.emplace<tag::Ball>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);
		auto& velocity = registry.emplace<physics::Velocity>(entity);

		sf::Vector2f direction = sf::Vector2f(Random::Range(-0.5f, 0.5f), Random::Range(-1.0f, 0.5f));
		direction = sf::Vector2f(0.f, -1.f);
		direction = VectorHelper::Normalize(direction);

		collider.m_Extents = m_BallSettings.size * 0.5f;
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "ball_0";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Blue);
		sprite.setOrigin(m_BallSettings.size * 0.5f);
		sprite.setSize(m_BallSettings.size);
		transform.m_Translate = m_BallSettings.position;
		velocity.m_Velocity = direction * m_BallSettings.velocityMin;
	}

	// bricks
	int index = 0;
	for (const BrickSettings& settings : m_BrickSettings)
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::Brick>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = settings.size * 0.5f;
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "brick_" + std::to_string(index++);
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Green);
		sprite.setOrigin(settings.size * 0.5f);
		sprite.setSize(settings.size);
		transform.m_Translate = settings.position;
	}

	// camera
	{
		entt::entity entity = registry.create();
		auto& camera = registry.emplace<core::Camera>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		camera.m_Size = sf::Vector2f(Screen::width, Screen::height);
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "camera_0";
		name.m_Name += " (" + level.m_Name + ")";
		transform.m_Translate = sf::Vector2f(0.f, 0.f);
	}

	// paddles
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::Paddle>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = m_PaddleSettings.size * 0.5f;
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "paddle_0";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::White);
		sprite.setOrigin(m_PaddleSettings.size * 0.5f);
		sprite.setSize(m_PaddleSettings.size);
		transform.m_Translate = m_PaddleSettings.position;
	}

	// respawners
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::RespawnZone>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = sf::Vector2f(800.f, 20.f);
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "respawn_zone_0";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Transparent);
		sprite.setOrigin(collider.m_Extents);
		sprite.setSize(collider.m_Extents * 2.f);
		transform.m_Translate = sf::Vector2f(0.f, Screen::height * 0.5f);
	}

	// wall top
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::Wall>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = sf::Vector2f(800.f, 20.f);
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "wall_0";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Transparent);
		sprite.setOrigin(collider.m_Extents);
		sprite.setSize(collider.m_Extents * 2.f);
		transform.m_Translate = sf::Vector2f(0.f, Screen::height * -0.5f);
	}

	// wall left
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::Wall>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = sf::Vector2f(20.f, 600.f);
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "wall_1";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Transparent);
		sprite.setOrigin(collider.m_Extents);
		sprite.setSize(collider.m_Extents * 2.f);
		transform.m_Translate = sf::Vector2f(Screen::width * -0.5f, 0.f);
	}

	// wall right
	{
		entt::entity entity = registry.create();
		auto& collider = registry.emplace<physics::Collider>(entity);
		auto& level = registry.emplace<core::Level>(entity);
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sprite = registry.emplace<sf::RectangleShape>(entity);
		auto& tag = registry.emplace<tag::Wall>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		collider.m_Extents = sf::Vector2f(20.f, 600.f);
		level.m_Name = levelName;
		level.m_Path = levelPath;
		name.m_Name = "wall_2";
		name.m_Name += " (" + level.m_Name + ")";
		sprite.setFillColor(sf::Color::Transparent);
		sprite.setOrigin(collider.m_Extents);
		sprite.setSize(collider.m_Extents * 2.f);
		transform.m_Translate = sf::Vector2f(Screen::width * 0.5f, 0.f);
	}

	// map
	{
		m_lives = m_MapSettings.lives;
		m_score = m_MapSettings.score;

		//HACK:
		UpdateLives(0);
		UpdateScore(0);
	}
}

void Map::Unload(entt::registry& registry)
{
	const auto view = registry.view<core::Level>();
	for (const entt::entity& entity : view)
	{
		registry.destroy(entity);
	}
}

void Map::UpdateLives(int value)
{
	m_lives += value;

	char buffer[10];
	_itoa_s(m_lives, buffer, 10);
	sf::String text(buffer);
	m_livesText.setString(text);

	if (m_lives <= 0)
	{
		Game::Instance().Pause();
	}
}

void Map::UpdateScore(int value)
{
	m_score += value;

	char buffer[10];
	_itoa_s(m_score, buffer, 10);
	sf::String text(buffer);
	m_scoreText.setString(text);
}

void Map::LoadSettings(const rapidjson::Value& value)
{
	LoadBallSettings(value);
	LoadBrickSettings(value);
	LoadMapSettings(value);
	LoadPaddleSettings(value);
}

void Map::LoadBallSettings(const rapidjson::Value& value)
{
	const char* member = "balls";
	if (value.HasMember(member) == true && value[member].IsString() == true)
	{
		rapidjson::Document settings;
		const char* filepath = value[member].GetString();
		if (JsonHelper::LoadDocument(filepath, settings) == true)
		{
			m_BallSettings.Parse(settings);
		}
	}
}

void Map::LoadBrickSettings(const rapidjson::Value& value)
{
	const char* member = "bricks";
	if (value.HasMember(member) == true && value[member].IsString() == true)
	{
		rapidjson::Document settings;
		const char* filepath = value[member].GetString();
		if (JsonHelper::LoadDocument(filepath, settings) == true)
		{
			if (settings.HasMember(member) == true)
			{
				const rapidjson::Value& objectArray = settings[member];
				if (objectArray.IsArray() == true)
				{
					m_BrickSettings.clear();
					for (rapidjson::SizeType i = 0; i < objectArray.Size(); ++i)
					{
						BrickSettings brickSetting;
						brickSetting.Parse(objectArray[i]);
						m_BrickSettings.push_back(brickSetting);
					}
				}
			}
		}
	}
}

void Map::LoadMapSettings(const rapidjson::Value& value)
{
	// map settings
	m_MapSettings.Parse(value);
}

void Map::LoadPaddleSettings(const rapidjson::Value& value)
{
	const char* member = "paddles";
	if (value.HasMember(member) == true && value[member].IsString() == true)
	{
		rapidjson::Document settings;
		const char* filepath = value[member].GetString();
		if (JsonHelper::LoadDocument(filepath, settings) == true)
		{
			m_PaddleSettings.Parse(settings);
		}
	}
}
