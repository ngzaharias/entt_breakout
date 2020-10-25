#include "VectorHelper.hpp"

#include <cmath>

namespace VectorHelper
{
	float Dot(const sf::Vector2f& a, const sf::Vector2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	float Length(const sf::Vector2f& value)
	{
		return std::sqrt(value.x * value.x + value.y * value.y);
	}

	sf::Vector2f Normalize(const sf::Vector2f& value)
	{
		//#todo: handle (0,0) vector?
		return value / Length(value);
	}

	sf::Vector2f Reflect(const sf::Vector2f& vector, const sf::Vector2f& normal)
	{
		// -2 * (V dot N)*N + V
		float dot2 = Dot(vector, normal) * -2.0f;
		return (dot2 * normal) + vector;
	}
}
