#include "EnttDebugger.hpp"

#include "StringHelper.hpp"

#include "Components/Camera.hpp"
#include "Components/Collider.hpp"
#include "Components/Level.hpp"
#include "Components/Name.hpp"
#include "Components/Rigidbody.hpp"
#include "Components/Sprite.hpp"
#include "Components/Tags.hpp"
#include "Components/Transform.hpp"
#include "Components/Velocity.hpp"

#include <iostream>
#include <entt/entt.hpp>
#include <imgui/imgui.h>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

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

debug::EnttDebugger::EnttDebugger()
	: m_ComponentInfo()
	, m_ComponentWidgets()
	, m_ComponentSettings()
	, m_EntitySettings()
	, m_EntitySelected(entt::null)
	, m_EntityInfo()
	, m_EntityOrphans()
	, m_IsWindowVisible(true)
{
}

debug::EnttDebugger::~EnttDebugger()
{
}

void debug::EnttDebugger::Initialise(entt::registry& registry)
{
	RegisterComponent<core::Camera>([](entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<core::Camera>(entity);
		if (ImGui::CollapsingHeader("Size"))
			ImGui::DragFloat2("", &component.m_Size.x);
	});

	RegisterComponent<core::Camera>([](entt::registry& registry, entt::entity& entity)
	{
		auto& component = registry.get<core::Camera>(entity);
		if (ImGui::CollapsingHeader("Size"))
			ImGui::DragFloat2("", &component.m_Size.x);
	});

	RegisterComponent<core::Level>([](entt::registry & registry, entt::entity & entity)
	{
		auto& component = registry.get<core::Level>(entity);
		if (ImGui::CollapsingHeader("Name"))
			ImGui::Text("%s", component.m_Name.c_str());
		if (ImGui::CollapsingHeader("Path"))
			ImGui::Text("%s", component.m_Path.c_str());
	});

	RegisterComponent<core::Transform>([](entt::registry& registry, entt::entity& entity)
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
	});

	RegisterComponent<debug::Name>([](entt::registry & registry, entt::entity & entity)
	{
		auto& component = registry.get<debug::Name>(entity);
		ImGui::Text("%s", component.m_Name.c_str());
	});

	RegisterComponent<physics::Collider>([](entt::registry & registry, entt::entity & entity)
	{
		auto& component = registry.get<physics::Collider>(entity);

		if (ImGui::CollapsingHeader("Extents"))
			ImGui::DragFloat2("", &component.m_Extents.x);
	});

	RegisterComponent<physics::Velocity>([](entt::registry & registry, entt::entity & entity)
	{
		auto& component = registry.get<physics::Velocity>(entity);

		if (ImGui::CollapsingHeader("Velocity"))
			ImGui::DragFloat2("", &component.m_Velocity.x);
	});

	RegisterComponent<render::Sprite>([](entt::registry & registry, entt::entity & entity)
	{
		auto& component = registry.get<render::Sprite>(entity);

		if (ImGui::CollapsingHeader("Fill Color"))
		{
			auto color = component.m_Shape.getFillColor();
			float values[4] = { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };
			ImGui::ColorEdit4("", values, ImGuiColorEditFlags_Uint8);
			color.r = static_cast<sf::Uint8>(values[0] * 255.f);
			color.g = static_cast<sf::Uint8>(values[1] * 255.f);
			color.b = static_cast<sf::Uint8>(values[2] * 255.f);
			color.a = static_cast<sf::Uint8>(values[3] * 255.f);
			component.m_Shape.setFillColor(color);
		}

		if (ImGui::CollapsingHeader("Outline Color"))
		{
			auto color = component.m_Shape.getOutlineColor();
			float values[4] = { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };
			ImGui::ColorEdit4("", values, ImGuiColorEditFlags_Uint8);
			color.r = static_cast<sf::Uint8>(values[0] * 255.f);
			color.g = static_cast<sf::Uint8>(values[1] * 255.f);
			color.b = static_cast<sf::Uint8>(values[2] * 255.f);
			color.a = static_cast<sf::Uint8>(values[3] * 255.f);
			component.m_Shape.setOutlineColor(color);
		}

		if (ImGui::CollapsingHeader("Outline Thickness"))
		{
			float value = component.m_Shape.getOutlineThickness();
			ImGui::DragFloat("", &value);
			component.m_Shape.setOutlineThickness(value);
		}

		if (ImGui::CollapsingHeader("Size"))
		{
			auto size = component.m_Shape.getSize();
			float values[2] = { size.x, size.y };
			ImGui::DragFloat2("Size", values);
			size.x = values[0];
			size.y = values[1];
			component.m_Shape.setSize(size);
		}
	});
}

void debug::EnttDebugger::Destory(entt::registry& registry)
{
}

void debug::EnttDebugger::Update(entt::registry& registry, const sf::Time& time)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
		m_IsWindowVisible = !m_IsWindowVisible;

	if (m_ComponentSettings.IsAutoRefreshEnabled || m_ComponentSettings.IsRefreshRequested)
	{
		m_ComponentInfo.clear();
		auto itr = registry.debug.begin();
		auto end = registry.debug.end();
		for (; itr != end; ++itr)
		{
			string::TrimLeft(itr->name, "class ");
			string::TrimLeft(itr->name, "struct ");
			m_ComponentInfo.push_back({ itr->name, itr->type_id });
		}

		std::sort(m_ComponentInfo.begin(), m_ComponentInfo.end(),
			[](const auto& a, const auto& b)
			{
				return a.Name < b.Name;
			});
	}

	if (m_EntitySettings.IsAutoRefreshEnabled || m_EntitySettings.IsRefreshRequested)
	{
		m_EntityInfo.clear();
		m_EntityOrphans.clear();

		if (!m_ComponentSettings.FilterTypes.empty())
		{
			const auto& list = m_ComponentSettings.FilterTypes;
			const auto& view = registry.runtime_view(list.cbegin(), list.cend());
			for (const entt::entity& entity : view)
			{
				m_EntityInfo.emplace_back(EntityInfo{ DebugName(registry, entity), entity });
			}
		}
		else
		{
			registry.each([&](const entt::entity& entity)
			{
				m_EntityInfo.emplace_back(EntityInfo{ DebugName(registry, entity), entity });
			});
		}

		std::sort(m_EntityInfo.begin(), m_EntityInfo.end(),
			[](const auto& a, const auto& b)
			{
				return a.Name < b.Name;
			});

		registry.orphans([&](const entt::entity& entity)
		{
			m_EntityOrphans.emplace(entity);
		});
	}
}

void debug::EnttDebugger::Render(entt::registry& registry)
{
	if (!m_IsWindowVisible)
		return;

	ImGui::Begin("Entt Debugger", &m_IsWindowVisible);
	if (ImGui::BeginChild("left", { ImGui::GetWindowWidth() * 0.5f, 0 }, false))
	{
		RenderComponents(registry);
		RenderEntities(registry);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("right", { ImGui::GetWindowWidth() * 0.5f - 23.f, 0 }, false))
	{
		RenderSelected(registry);
	}
	ImGui::EndChild();
	ImGui::End();
}

void debug::EnttDebugger::RenderComponents(entt::registry& /*registry*/)
{
	if (ImGui::BeginChild("components", { 0, ImGui::GetWindowHeight() * 0.5f }, true))
	{
		if (ImGui::BeginChild("header", { 0, 60.f }))
		{
			ImGui::Text("Components: ");

			ImGui::InputText("", m_ComponentSettings.FilterText);
			ImGui::SameLine();
			if (ImGui::Button("X"))
				m_ComponentSettings.FilterText = "";

			ImGui::Checkbox("Auto Refresh", &m_ComponentSettings.IsAutoRefreshEnabled);
			ImGui::SameLine();
			if (ImGui::Button("Refresh"))
				m_ComponentSettings.IsRefreshRequested = true;
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				m_ComponentInfo.clear();
				m_ComponentSettings.FilterTypes.clear();
			}
		}
		ImGui::EndChild();

		ImGui::Separator();

		if (ImGui::BeginChild("body", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			std::vector<std::string> substrings = string::Split(m_ComponentSettings.FilterText, " ");

			for (const auto& info : m_ComponentInfo)
			{
				if (!m_ComponentSettings.FilterText.empty() && !string::ContainsAll(info.Name, substrings))
					continue;

				bool isInList = m_ComponentSettings.FilterTypes.count(info.TypeId);
				bool isActive = isInList;

				ImGui::Checkbox(info.Name.c_str(), &isActive);

				// add
				if (!isInList && isActive)
				{
					m_ComponentSettings.FilterTypes.emplace(info.TypeId);
				}
				// remove
				else if (isInList && !isActive)
				{
					m_ComponentSettings.FilterTypes.erase(info.TypeId);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
}

void debug::EnttDebugger::RenderEntities(entt::registry& registry)
{
	if (ImGui::BeginChild("entities", { 0, ImGui::GetWindowHeight() * 0.5f - 3.f }, true, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (ImGui::BeginChild("header", { 0, 60.f }))
		{
			ImGui::Text(m_EntitySettings.IsShowingOrphans ? "Orphans: " : "Entities: ");

			ImGui::InputText("", m_EntitySettings.FilterText);
			ImGui::SameLine();
			if (ImGui::Button("X"))
				m_EntitySettings.FilterText = "";

			ImGui::Checkbox("Auto Refresh", &m_EntitySettings.IsAutoRefreshEnabled);
			ImGui::SameLine();
			if (ImGui::Button("Refresh"))
				m_EntitySettings.IsRefreshRequested = true;
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
				m_EntityInfo.clear();

			//ImGui::SameLine();
			ImGui::Checkbox("Show Orphans", &m_EntitySettings.IsShowingOrphans);
		}
		ImGui::EndChild();

		ImGui::Separator();

		if (ImGui::BeginChild("body", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			std::vector<std::string> substrings = string::Split(m_EntitySettings.FilterText, " ");
			if (m_EntitySettings.IsShowingOrphans)
			{
				for (const auto& entity : m_EntityOrphans)
				{
					bool isActive = entity == m_EntitySelected;
					ImGui::PushID(static_cast<int>(entity));
					if (ImGui::Checkbox("<unknown>", &isActive))
						m_EntitySelected = m_EntitySelected == entity ? entt::null : entity;
					ImGui::PopID();
				}
			}
			else
			{
				for (const auto& info : m_EntityInfo)
				{
					bool isActive = info.Entity == m_EntitySelected;
					if (!registry.valid(info.Entity))
						continue;

					if (!m_EntitySettings.FilterText.empty() && !string::ContainsAll(info.Name, substrings))
						continue;

					ImGui::PushID(static_cast<int>(info.Entity));
					if (ImGui::Checkbox(info.Name.c_str(), &isActive))
						m_EntitySelected = m_EntitySelected == info.Entity ? entt::null : info.Entity;
					ImGui::PopID();
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
}

void debug::EnttDebugger::RenderSelected(entt::registry& registry)
{
	if (ImGui::BeginChild("selected", { 0, ImGui::GetWindowHeight() }, true))
	{
		if (registry.valid(m_EntitySelected))
		{
			ImGui::PushID(static_cast<int>(m_EntitySelected));
			ImGui::Text("Name: %s", DebugName(registry, m_EntitySelected).c_str());
			ImGui::Text("ID:   %d", static_cast<int>(m_EntitySelected));

			ImGui::Separator();

			for (const auto& info : m_ComponentInfo)
			{
				if (!HasComponent(registry, m_EntitySelected, info.TypeId))
					continue;

				ImGui::PushID(info.TypeId);
				if (ImGui::CollapsingHeader(info.Name.c_str()))
				{
					ImGui::Indent();
					if (m_ComponentWidgets.count(info.TypeId))
					{
						const auto& callback = m_ComponentWidgets[info.TypeId];
						callback(registry, m_EntitySelected);
					}
					ImGui::Unindent();
				}
				ImGui::PopID();
			}
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
}
