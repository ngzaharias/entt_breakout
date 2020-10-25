#include "SoundSystem.hpp"

#include "Random.hpp"

#include "Components/Name.hpp"
#include "Components/Sound.hpp"
#include "Components/Transform.hpp"

#include <iostream>
#include <entt/entt.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace
{
	constexpr char* s_SoundPath_Impacts[] =
	{
		//"Assets/Audio/S_Example.ogg"
		"Assets/Audio/S_Impact_Generic_001.ogg"
		, "Assets/Audio/S_Impact_Generic_002.ogg"
		, "Assets/Audio/S_Impact_Generic_003.ogg"
		, "Assets/Audio/S_Impact_Generic_004.ogg"
		, "Assets/Audio/S_Impact_Generic_005.ogg"
	};
}

audio::SoundSystem::SoundSystem()
{
}

audio::SoundSystem::~SoundSystem()
{
}

void audio::SoundSystem::Initialize()
{
	for (const char* path : s_SoundPath_Impacts)
	{
		sf::SoundBuffer buffer;
		if (LoadResource(path, buffer))
			m_Impacts.push_back(buffer);
	}
}

void audio::SoundSystem::Destroy()
{
}

void audio::SoundSystem::Update(entt::registry& registry, const sf::Time& time)
{
	for (const audio::Request& request : m_Requests)
	{
		const char* soundName = "<unknown>";
		sf::SoundBuffer* soundBuffer = nullptr;
		switch (request.m_Name)
		{
		case Name::Impact:
		{
			const int count = static_cast<int>(m_Impacts.size());
			const int index = random::Range(0, count - 1);
			soundBuffer = &m_Impacts[index];
			soundName = "Impact";
		} break;
		}

		if (!soundBuffer)
			continue;

		entt::entity entity = registry.create();
		auto& name = registry.emplace<debug::Name>(entity);
		auto& sound = registry.emplace<audio::Sound>(entity);
		auto& transform = registry.emplace<core::Transform>(entity);

		name.m_Name = soundName;
		sound.m_Sound = m_SoundPool.RequestObject();
		sound.m_Sound->setBuffer(*soundBuffer);
		sound.m_Sound->setVolume(20.f);
		sound.m_Sound->play();
	}
	m_Requests.clear();

	const auto view = registry.view<audio::Sound>();
	for (const entt::entity& entity : view)
	{
		auto& sound = registry.get<audio::Sound>(entity);
		if (sound.m_Sound->getStatus() == sf::Sound::Stopped)
		{
			m_SoundPool.ReleaseObject(sound.m_Sound);
			registry.destroy(entity);
		}
	}
}

void audio::SoundSystem::PlaySound(const audio::Name& name)
{
	m_Requests.push_back({ name });
}

bool audio::SoundSystem::LoadResource(const std::string& path, sf::SoundBuffer& out_Buffer)
{
	return out_Buffer.loadFromFile(path);
}
