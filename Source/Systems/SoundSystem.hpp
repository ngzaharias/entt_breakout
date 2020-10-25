#pragma once

#include "CircularBuffer.hpp"
#include "ObjectPool.hpp"

#include "Components/Sound.hpp"

#include <string>
#include <vector>
#include <entt/fwd.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace sf
{
	class Sound;
	class Time;
}

namespace audio
{
	// #todo: replace with FixedString
	enum class Name
	{
		Unknown,
		Impact,
	};

	struct Request
	{
		audio::Name m_Name = Name::Unknown;
	};

	class SoundSystem
	{
	public:
		SoundSystem();
		~SoundSystem();

		void Initialize();
		void Destroy();

		void Update(entt::registry& registry, const sf::Time& time);

		void PlaySound(const audio::Name& name);

	private:
		bool LoadResource(const std::string& path, sf::SoundBuffer& out_Buffer);

	private:
		// #todo: move to resource manager
		// #todo: resizing of the vector will invalidate any sounds that use the buffers
		std::vector<sf::SoundBuffer> m_Impacts;

		std::vector<audio::Request> m_Requests;

		ObjectPool<sf::Sound, 128> m_SoundPool;
	};
};