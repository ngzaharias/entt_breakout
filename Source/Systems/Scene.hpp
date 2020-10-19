#pragma once

#include <entt/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <entt/signal/sigh.hpp>

namespace sf
{
	class Time;
}

namespace physics
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update(entt::registry& registry, const sf::Time& time);

		entt::sigh<void(const entt::entity&, const entt::entity&)> m_OnCollisionSignal;
	};
};