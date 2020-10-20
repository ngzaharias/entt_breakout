#pragma once

namespace
{
	template <class Component>
	void ComponentWidget([[maybe_unused]] entt::registry&, [[maybe_unused]] entt::entity&) { }
}

template <class Component>
void debug::EnttDebugger::RegisterComponent(const std::string& name)
{
	const entt::id_type typeId = entt::type_info<Component>::id();
	m_ComponentsRegistered.push_back({ typeId, name, ComponentWidget<Component> });
}