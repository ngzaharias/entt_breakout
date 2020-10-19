#include "Level.hpp"

#include "Components/Transform.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

game::Level::Level()
{
}

game::Level::~Level()
{
}

void game::Level::Update(entt::registry& registry, const sf::Time& time)
{
	const auto view = registry.view<sf::RectangleShape, core::Transform>();
	for (const entt::entity& entity : view)
	{
	}
}
