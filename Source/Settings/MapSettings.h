#pragma once

#include <rapidjson/document.h>
#include <string>

struct MapSettings
{
	void Parse(const rapidjson::Value& value);

	int lives = 3;
	int score = 0;
	std::string fontpath = "Assets/Fonts/kenpixel_square.ttf";
};
