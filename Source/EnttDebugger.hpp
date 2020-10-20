#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <entt/fwd.hpp>

namespace sf
{
	class Time;
}

namespace debug
{
	struct ComponentInfo
	{
		using Callback = std::function<void(entt::registry&, entt::entity&)>;

		entt::id_type TypeId;
		std::string Name;
		Callback Widget;
	};

	struct Filters
	{
		std::set<entt::id_type> ComponentTypes;
		std::string ComponentText = "";
		std::string EntityText = "";
	};

	class EnttDebugger
	{
	public:
		EnttDebugger();
		~EnttDebugger();

		void Initialise(entt::registry& registry);
		void Destory(entt::registry& registry);

		void Update(entt::registry& registry, const sf::Time& time);
		void Render(entt::registry& registry);

	private:
		template <class Component>
		void RegisterComponent(const std::string& name);

	private:
		std::vector<ComponentInfo> m_ComponentsRegistered;
		std::set<entt::entity> m_EntitiesMatched;
		std::set<entt::entity> m_EntitiesOrphaned;
		entt::entity m_EntitySelected;

		Filters m_Filters;

		bool m_IsAutoRefreshEnabled;
		bool m_IsRefreshRequested;
		bool m_IsShowingOrphans;
		bool m_IsWindowVisible;
	};
}

#include "EnttDebugger.inl"