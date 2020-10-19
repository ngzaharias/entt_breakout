#pragma once

#include "BallSettings.hpp"
#include "BrickSettings.hpp"
#include "MapSettings.hpp"
#include "PaddleSettings.hpp"

#include <entt/fwd.hpp>
#include <rapidjson/document.h>
#include <SFML/Graphics/Text.hpp>

#include <vector>

namespace sf
{
	class RenderWindow;
	class Time;
}

class Map
{
public:
	Map();
	~Map();

public:
	void Update(entt::registry& registry, const sf::Time& time);
	void Draw(sf::RenderWindow* window);

	void Load(entt::registry& registry);
	void Unload(entt::registry& registry);

	void UpdateLives(int value);
	void UpdateScore(int value);

private:
	void LoadSettings(const rapidjson::Value& value);
	void LoadBallSettings(const rapidjson::Value& value);
	void LoadBrickSettings(const rapidjson::Value& value);
	void LoadMapSettings(const rapidjson::Value& value);
	void LoadPaddleSettings(const rapidjson::Value& value);

public:
	BallSettings m_BallSettings;
	std::vector<BrickSettings> m_BrickSettings;
	MapSettings m_MapSettings;
	PaddleSettings m_PaddleSettings;

private:
	sf::Font m_font;
	sf::Text m_infoText;
	sf::Text m_livesText;
	sf::Text m_scoreText;

	int m_lives = 0;
	int m_score = 0;
};
