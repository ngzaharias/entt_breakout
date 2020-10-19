#include "Renderer.hpp"

#include "Components/Camera.hpp"
#include "Components/Transform.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

render::Renderer::Renderer()
{
}

render::Renderer::~Renderer()
{
}

void render::Renderer::Update(entt::registry& registry, sf::RenderWindow* window)
{
	const auto cameraView = registry.view<core::Camera, core::Transform>();
	for (const entt::entity& cameraEntity : cameraView)
	{
		{
			auto& camera = cameraView.get<core::Camera>(cameraEntity);
			auto& transform = cameraView.get<core::Transform>(cameraEntity);
			camera.m_View.setCenter(transform.m_Translate);
			camera.m_View.setRotation(transform.m_Rotate);
			window->setView(camera.m_View);
		}

		const auto renderView = registry.view<sf::RectangleShape, core::Transform>();
		for (const entt::entity& renderEntity : renderView)
		{
			auto& sprite = renderView.get<sf::RectangleShape>(renderEntity);
			auto& transform = renderView.get<core::Transform>(renderEntity);

			sprite.setPosition(transform.m_Translate);
			sprite.setRotation(transform.m_Rotate);
			sprite.setScale(transform.m_Scale);
			window->draw(sprite);
		}
	}
}
