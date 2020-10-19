#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace render
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Update(entt::registry& registry, sf::RenderWindow* window);
	};
};