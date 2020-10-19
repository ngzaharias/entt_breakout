#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class Time;
}

namespace game
{
	class Level
	{
	public:
		Level();
		~Level();

		void Update(entt::registry& registry, const sf::Time& time);
	};
};