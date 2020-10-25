#pragma once

#include "BallSettings.hpp"
#include "BrickSettings.hpp"
#include "LevelSettings.hpp"
#include "PaddleSettings.hpp"

#include <entt/fwd.hpp>
#include <rapidjson/document.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
	class RenderWindow;
	class Time;
}

namespace core
{
	static constexpr char* s_LevelPath = "Assets/Settings/Maps/BasicMap.json";

	class LevelSystem
	{
	public:
		LevelSystem();
		~LevelSystem();

		void Update(entt::registry& registry, const sf::Time& time);
		void Draw(sf::RenderWindow* window);

		bool Load(entt::registry& registry, const std::string& path);
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
		// #todo: move into save files
		BallSettings m_BallSettings;
		std::vector<BrickSettings> m_BrickSettings;
		LevelSettings m_MapSettings;
		PaddleSettings m_PaddleSettings;

	private:
		sf::Texture m_BallTexture;
		std::vector<sf::Texture> m_BrickTextures;
		sf::Texture m_PaddleTexture;

		// #todo: move out of level system
		sf::Font m_font;
		sf::Text m_infoText;
		sf::Text m_livesText;
		sf::Text m_scoreText;

		// #todo: move out of level system
		int m_lives = 0;
		int m_score = 0;
	};
};