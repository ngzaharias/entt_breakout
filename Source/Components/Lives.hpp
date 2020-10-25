#pragma once

#include <SFML/Graphics/Text.hpp>

namespace game
{
	struct Lives
	{
		sf::Text m_Text = { };
		int m_Value = 0;
	};
}