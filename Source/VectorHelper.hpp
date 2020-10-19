#pragma once

#include <SFML/System/Vector2.hpp>

namespace VectorHelper
{
	float Dot(const sf::Vector2f& a, const sf::Vector2f& b);
	float Magnitude(const sf::Vector2f& value);
	sf::Vector2f Normalize(const sf::Vector2f& value);
	sf::Vector2f Reflect(const sf::Vector2f& vector, const sf::Vector2f& normal);
}
