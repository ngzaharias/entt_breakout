#pragma once

namespace
{
	template <class Component>
	void ComponentWidget([[maybe_unused]] entt::registry&, [[maybe_unused]] entt::entity&) { }
}

template <class Component>
void debug::EnttDebugger::RegisterComponent(WidgetCallback&& callback)
{
	m_ComponentWidgets[entt::type_info<Component>::id()] = callback;
}