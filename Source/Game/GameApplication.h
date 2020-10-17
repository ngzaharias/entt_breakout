#pragma once

#include "Application.h"

namespace sf
{
	class Clock;
	class RenderWindow;
}

class Game;

class GameApplication : public Application
{
public:
	GameApplication();
	virtual ~GameApplication() override;

protected:
	virtual bool Initialise(int agrc, char* argv[]) override;
	virtual void Destroy() override;

	virtual bool Update() override;
	virtual bool Draw() override;

protected:
	sf::Clock* m_Clock;
	sf::RenderWindow* m_Window;

	Game* m_Game;
};
