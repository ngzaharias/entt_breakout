#include "StringHelper.hpp"

#include <algorithm>

bool string::Contains(const std::string& string, const std::string& substring)
{
	std::string a = string::ToUpper(string);
	std::string b = string::ToUpper(substring);
	return a.find(b) != std::string::npos;
}

bool string::Contains(const std::string& string, const std::vector<std::string>& substrings)
{
	std::string a = string::ToUpper(string), b;
	for (const std::string& substring : substrings)
	{
		b = string::ToUpper(substring);
		if (a.find(b) != std::string::npos)
			return true;
	}
	return false;
}

void string::Split(const std::string& string, std::vector<std::string>& substrings, char deliminator /*= ' '*/)
{
	std::string::size_type begin = 0;
	std::string::size_type end = 0;
	std::string substring;
	for (; (end = string.find(deliminator, end)) != std::string::npos; ++end)
	{
		substring = string.substr(begin, end - begin);
		substrings.push_back(substring);
		begin = end + 1;
	}

	substring = string.substr(begin, end - begin);
	substrings.push_back(substring);
}

std::string string::ToLower(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::tolower(c); });
	return string;
}

std::string string::ToUpper(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::toupper(c); });
	return string;
}
