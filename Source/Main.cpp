//#include "Application.h"
//#include "Game/GameApplication.h"
//
//#include <direct.h>
//
//Application* application = nullptr;
//
//int main(int agrc, char* argv[])
//{
//	_chdir("../../");
//
//	application = new GameApplication();
//	application->Run(agrc, argv);
//	delete(application);
//	application = nullptr;
//
//	return 0;
//}

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(1777, 1000), "ImGui + SFML = <3");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	sf::View centreView;
	sf::Vector2u size = window.getSize();
	centreView.setSize(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));
	centreView.setCenter(0, 0);
	window.setView(centreView);

	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(sf::Vector2f(-50.f, -50.f));

	sf::Clock deltaClock;
	while (window.isOpen()) 
	{
		sf::Event event;
		while (window.pollEvent(event)) 
		{
			ImGui::SFML::ProcessEvent(event);
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::ShowDemoWindow();

		window.clear();
		window.draw(shape);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}