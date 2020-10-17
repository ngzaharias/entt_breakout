#include "GameApplication.h"

#include "Game.h"
#include "Engine/Physics.h"
#include "Engine/Screen.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include <random>
#include <time.h>

GameApplication::GameApplication()
{
	m_Clock = new sf::Clock();

	sf::VideoMode videoMode = sf::VideoMode(Screen::width, Screen::height);
	m_Window = new sf::RenderWindow(videoMode, "Breakout: Nicholas Zaharias");
	Screen::SetWindow(m_Window);

	m_Game = new Game();

	srand((unsigned int)time(NULL));
}

GameApplication::~GameApplication()
{
	delete m_Clock;
	delete m_Game;
	delete m_Window;

	Screen::SetWindow(nullptr);
}

bool GameApplication::Initialise(int agrc, char* argv[])
{
	ImGui::SFML::Init(*m_Window);
	m_Game->Initialise();
	return true;
}

void GameApplication::Destroy()
{
	ImGui::SFML::Shutdown();
	m_Game->Destroy();
}

bool GameApplication::Update()
{
	if (!m_Window->isOpen())
		return false;

	sf::Time delta = m_Clock->restart();

	sf::Event event;
	while (m_Window->pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		switch (event.type)
		{
		case sf::Event::Closed:
			m_Window->close();
			return false;
		};
	}

	ImGui::SFML::Update(*m_Window, delta);
	m_Game->Update(delta.asSeconds());

	return true;
}

bool GameApplication::Draw()
{
	//ImGui::ShowDemoWindow();

	m_Window->clear();
	m_Game->Draw(m_Window);
	ImGui::SFML::Render(*m_Window);
	m_Window->display();

	return true;
}