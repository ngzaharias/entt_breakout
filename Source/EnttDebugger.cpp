#include "EnttDebugger.hpp"

#include "StringHelper.hpp"

#include "Components/Camera.hpp"
#include "Components/Collider.hpp"
#include "Components/Level.hpp"
#include "Components/Name.hpp"
#include "Components/Rigidbody.hpp"
#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include <entt/entt.hpp>
#include <imgui/imgui.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

#define REGISTER_COMPONENT(component) RegisterComponent<component>(#component)

namespace ImGui
{
	void InputText(const char* label, std::string& string)
	{
		const size_t size = 256;
		char* buffer = new char[size];
		strncpy_s(buffer, size, string.c_str(), size);

		ImGui::InputText(label, buffer, size);
		string = buffer;
	}
}

namespace
{
	std::string DebugName(const entt::registry& registry, const entt::entity& entity)
	{
		if (const auto& component = registry.try_get<debug::Name>(entity))
			return component->m_Name;
		return "<unknown> (unknown)";
	}

	bool HasComponent(const entt::registry& registry, const entt::entity& entity, const entt::id_type& typeId)
	{
		entt::id_type types[] = { typeId };
		return registry.runtime_view(std::cbegin(types), std::cend(types)).contains(entity);
	}
}

namespace
{
	template <>
	void ComponentWidget<core::Camera>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<core::Camera>(entity);
		if (ImGui::CollapsingHeader("Size"))
			ImGui::DragFloat2("", &component.m_Size.x);
	}

	template <>
	void ComponentWidget<core::Level>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<core::Level>(entity);
		if (ImGui::CollapsingHeader("Name"))
			ImGui::Text("%s", component.m_Name.c_str());
		if (ImGui::CollapsingHeader("Path"))
			ImGui::Text("%s", component.m_Path.c_str());
	}

	template <>
	void ComponentWidget<core::Transform>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<core::Transform>(entity);

		ImGui::PushID("Translate");
		if (ImGui::CollapsingHeader("Translate"))
			ImGui::DragFloat2("", &component.m_Translate.x);
		ImGui::PopID();

		ImGui::PushID("Rotate");
		if (ImGui::CollapsingHeader("Rotate"))
			ImGui::DragFloat("", &component.m_Rotate);
		ImGui::PopID();

		ImGui::PushID("Scale");
		if (ImGui::CollapsingHeader("Scale"))
			ImGui::DragFloat2("", &component.m_Scale.x);
		ImGui::PopID();
	}

	template <>
	void ComponentWidget<debug::Name>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<debug::Name>(entity);
		ImGui::Text("%s", component.m_Name.c_str());
	}

	template <>
	void ComponentWidget<physics::Collider>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<physics::Collider>(entity);

		if (ImGui::CollapsingHeader("Extents"))
			ImGui::DragFloat2("", &component.m_Extents.x);
	}

	template <>
	void ComponentWidget<physics::Velocity>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<physics::Velocity>(entity);

		if (ImGui::CollapsingHeader("Velocity"))
			ImGui::DragFloat2("", &component.m_Velocity.x);
	}

	template <>
	void ComponentWidget<sf::RectangleShape>(entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<sf::RectangleShape>(entity);

		if (ImGui::CollapsingHeader("Fill Color"))
		{
			auto color = component.getFillColor();
			float values[4] = { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };
			ImGui::ColorEdit4("", values, ImGuiColorEditFlags_Uint8);
			color.r = static_cast<sf::Uint8>(values[0] * 255.f);
			color.g = static_cast<sf::Uint8>(values[1] * 255.f);
			color.b = static_cast<sf::Uint8>(values[2] * 255.f);
			color.a = static_cast<sf::Uint8>(values[3] * 255.f);
			component.setFillColor(color);
		}

		if (ImGui::CollapsingHeader("Outline Color"))
		{
			auto color = component.getOutlineColor();
			float values[4] = { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };
			ImGui::ColorEdit4("", values, ImGuiColorEditFlags_Uint8);
			color.r = static_cast<sf::Uint8>(values[0] * 255.f);
			color.g = static_cast<sf::Uint8>(values[1] * 255.f);
			color.b = static_cast<sf::Uint8>(values[2] * 255.f);
			color.a = static_cast<sf::Uint8>(values[3] * 255.f);
			component.setOutlineColor(color);
		}

		if (ImGui::CollapsingHeader("Outline Thickness"))
		{
			float value = component.getOutlineThickness();
			ImGui::DragFloat("", &value);
			component.setOutlineThickness(value);
		}

		if (ImGui::CollapsingHeader("Size"))
		{
			auto size = component.getSize();
			float values[2] = { size.x, size.y };
			ImGui::DragFloat2("Size", values);
			size.x = values[0];
			size.y = values[1];
			component.setSize(size);
		}
	}
}

debug::EnttDebugger::EnttDebugger()
	: m_ComponentsRegistered()
	, m_Filters()
	, m_EntitiesMatched()
	, m_EntitiesOrphaned()
	, m_EntitySelected(entt::null)
	, m_IsAutoRefreshEnabled(true)
	, m_IsRefreshRequested(false)
	, m_IsShowingOrphans(false)
	, m_IsWindowVisible(true)
{
}

debug::EnttDebugger::~EnttDebugger()
{
}

void debug::EnttDebugger::Initialise(entt::registry& registry)
{
	REGISTER_COMPONENT(core::Camera);
	REGISTER_COMPONENT(core::Level);
	REGISTER_COMPONENT(core::Transform);

	REGISTER_COMPONENT(debug::Name);

	REGISTER_COMPONENT(physics::Collider);
	REGISTER_COMPONENT(physics::Rigidbody);
	REGISTER_COMPONENT(physics::Velocity);

	REGISTER_COMPONENT(sf::RectangleShape);

	REGISTER_COMPONENT(tag::Ball);
	REGISTER_COMPONENT(tag::Brick);
	REGISTER_COMPONENT(tag::Paddle);
	REGISTER_COMPONENT(tag::RespawnZone);
	REGISTER_COMPONENT(tag::Wall);
}

void debug::EnttDebugger::Destory(entt::registry& registry)
{
}

void debug::EnttDebugger::Update(entt::registry& registry, const sf::Time& time)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
		m_IsWindowVisible = !m_IsWindowVisible;

	if (m_IsAutoRefreshEnabled || m_IsRefreshRequested)
	{
		m_EntitiesMatched.clear();
		m_EntitiesOrphaned.clear();

		if (!m_Filters.ComponentTypes.empty())
		{
			auto view = registry.runtime_view(m_Filters.ComponentTypes.begin(), m_Filters.ComponentTypes.end());
			for (const entt::entity& entity : view)
			{
				m_EntitiesMatched.emplace(entity);
			}
		}
		else
		{
			registry.each([&](const entt::entity& entity)
			{
				m_EntitiesMatched.emplace(entity);
			});
		}

		registry.orphans([&](const entt::entity& entity)
		{
			m_EntitiesOrphaned.emplace(entity);
		});

		m_EntitiesOrphaned;
	}
}

void debug::EnttDebugger::Render(entt::registry& registry)
{
	if (!m_IsWindowVisible)
		return;

	ImGui::Begin("Entt Debugger", &m_IsWindowVisible);

	if (ImGui::BeginChild("left", { ImGui::GetWindowWidth() * 0.5f, 0 }, false))
	{
		if (ImGui::BeginChild("filtering", { 0, ImGui::GetWindowHeight() * 0.5f }, true, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::Text("Filters: ");

			ImGui::InputText("", m_Filters.ComponentText);
			ImGui::SameLine();
			if (ImGui::Button("X"))
				m_Filters.ComponentText = "";

			if (ImGui::Button("Clear Components"))
				m_Filters.ComponentTypes.clear();

			ImGui::Separator();

			std::vector<std::string> substrings = string::Split(m_Filters.ComponentText, " ");
			for (const ComponentInfo& info : m_ComponentsRegistered)
			{
				if (!m_Filters.ComponentText.empty() && !string::ContainsAll(info.Name, substrings))
					continue;

				bool isInList = m_Filters.ComponentTypes.count(info.TypeId);
				bool isActive = isInList;

				ImGui::Checkbox(info.Name.c_str(), &isActive);

				// remove
				if (isInList && !isActive)
				{
					m_Filters.ComponentTypes.erase(info.TypeId);
				}
				// add
				else if (!isInList && isActive)
				{
					m_Filters.ComponentTypes.emplace(info.TypeId);
				}
			}
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("selection", { 0, ImGui::GetWindowHeight() * 0.5f - 4.f }, true, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::Text(m_IsShowingOrphans ? "Orphans: " : "Entities: ");

			ImGui::InputText("", m_Filters.EntityText);
			ImGui::SameLine();
			if (ImGui::Button("X"))
				m_Filters.EntityText = "";

			ImGui::Checkbox("Auto Refresh", &m_IsAutoRefreshEnabled);
			ImGui::SameLine();
			if (ImGui::Button("Refresh"))
				m_IsRefreshRequested = true;
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
				m_EntitiesMatched.clear();

			//ImGui::SameLine();
			ImGui::Checkbox("Show Orphans", &m_IsShowingOrphans);

			ImGui::Separator();

			std::vector<std::string> substrings = string::Split(m_Filters.EntityText, " ");
			auto& list = m_IsShowingOrphans ? m_EntitiesOrphaned : m_EntitiesMatched;
			for (const entt::entity& entity : list)
			{
				bool isActive = entity == m_EntitySelected;
				if (!registry.valid(entity))
					continue;

				const std::string name = DebugName(registry, entity);
				if (!m_Filters.EntityText.empty() && !string::ContainsAll(name, substrings))
					continue;

				ImGui::PushID(static_cast<int>(entity));
				if (ImGui::Checkbox(name.c_str(), &isActive))
					m_EntitySelected = m_EntitySelected == entity ? entt::null : entity;
				ImGui::PopID();
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("right", { ImGui::GetWindowWidth() * 0.5f - 23.f, 0 }, false))
	{
		if (ImGui::BeginChild("display", { 0, ImGui::GetWindowHeight() }, true))
		{
			if (registry.valid(m_EntitySelected))
			{
				ImGui::PushID(static_cast<int>(m_EntitySelected));
				ImGui::Text("Name: %s", DebugName(registry, m_EntitySelected).c_str());
				ImGui::Text("ID:   %d", static_cast<int>(m_EntitySelected));
				ImGui::NewLine();

				for (const ComponentInfo& info : m_ComponentsRegistered)
				{
					if (!HasComponent(registry, m_EntitySelected, info.TypeId))
						continue;

					ImGui::PushID(info.TypeId);
					if (ImGui::CollapsingHeader(info.Name.c_str()))
					{
						ImGui::Indent();
						ImGui::PushID("widget");
						info.Widget(registry, m_EntitySelected);
						ImGui::PopID();
						ImGui::Unindent();
					}
					ImGui::PopID();
				}
				ImGui::PopID();
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();

	ImGui::End();
}