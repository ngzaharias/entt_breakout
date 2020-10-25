#pragma once

#include "EnttDebugger.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SoundSystem.hpp"

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

class Map;

class Game final
{

public:
	Game();
	~Game();

public:
	void Initialise(entt::registry& registry);
	void Destroy(entt::registry& registry);

	void Update(entt::registry& registry, const sf::Time& time);
	void Draw(entt::registry& registry, sf::RenderWindow* window);

	void Pause();
	void Unpause();

private:
	void OnCollision(const entt::entity& entityA, const entt::entity& entityB);

public: 
	static Game& Instance();

public:
	entt::registry* m_Registry;

	debug::EnttDebugger m_EnttDebugger;
	core::LevelSystem m_LevelSystem;
	physics::PhysicsSystem m_PhysicsSystem;
	render::RenderSystem m_RenderSystem;
	audio::SoundSystem m_SoundSystem;

	bool m_IsPaused = false;

protected:
	static Game* s_Instance;
};
