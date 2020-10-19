#pragma once

#include <SFML/Graphics/Text.hpp>

namespace core
{
	struct Score
	{
		sf::Text m_Text = { };
		int m_Value = 0;
	};
}