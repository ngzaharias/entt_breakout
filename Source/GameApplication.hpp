#pragma once

#include "Application.hpp"

class Game;

class GameApplication final : public Application
{
public:
	GameApplication();
	~GameApplication() override;

protected:
	bool Initialise(int argc, char* argv[]) override;
	void Destroy() override;

	bool Update() override;
	bool Draw() override;

protected:
	Game* m_Game;
};
