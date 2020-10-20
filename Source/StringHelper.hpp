#pragma once

#include <string>
#include <vector>

namespace string
{
	bool Contains(const std::string& string, const std::string& substring);
	bool Contains(const std::string& string, const std::vector<std::string>& substrings);
	void Split(const std::string& string, std::vector<std::string>& substrings, char deliminator = ' ');

	std::string ToLower(std::string string);
	std::string ToUpper(std::string string);
}