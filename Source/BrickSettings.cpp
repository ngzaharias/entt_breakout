#include "BrickSettings.hpp"

#include "JsonHelper.hpp"

void BrickSettings::Parse(const rapidjson::Value& value)
{
	if (value.IsObject() == false)
		return;

	position.x = JsonHelper::ParseFloat(value, "position_x", position.x);
	position.y = JsonHelper::ParseFloat(value, "position_y", position.y);

	size.x = JsonHelper::ParseFloat(value, "size_x", size.x);
	size.y = JsonHelper::ParseFloat(value, "size_y", size.y);

	textureId = JsonHelper::ParseInt(value, "texture_id", textureId);
}
