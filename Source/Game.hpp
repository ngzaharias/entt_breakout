#pragma once

#include <entt/fwd.hpp>

#include "EnttDebugger.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/Scene.hpp"

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

	Map& GetMap() const;

private:
	void OnCollision(const entt::entity& entityA, const entt::entity& entityB);

public: 
	static Game& Instance();

public:
	entt::registry* m_Registry;
	physics::Scene m_Scene;
	render::Renderer m_Renderer;
	debug::EnttDebugger m_EnttDebugger;

	Map* m_Map;
	bool m_IsPaused = false;

protected:
	static Game* s_Instance;

};
