#include "StringHelper.hpp"

#include <algorithm>

bool string::Contains(std::string string, std::string substring)
{
	string = string::ToUpper(string);
	substring = string::ToUpper(substring);
	return string.find(substring) != std::string::npos;
}

bool string::ContainsAny(std::string string, const std::vector<std::string>& substrings)
{
	string = string::ToUpper(string);
	for (std::string substring : substrings)
	{
		substring = string::ToUpper(substring);
		if (string.find(substring) != std::string::npos)
			return true;
	}
	return false;
}

bool string::ContainsAll(std::string string, const std::vector<std::string>& substrings)
{
	string = string::ToUpper(string);
	for (std::string substring : substrings)
	{
		substring = string::ToUpper(substring);
		if (string.find(substring) == std::string::npos)
			return false;
	}
	return true;
}

std::vector<std::string> string::Split(const std::string& string, const std::string_view& delimiters)
{
	std::vector<std::string> substrings;
	std::string::size_type begin = string.find_first_not_of(delimiters, 0);
	std::string::size_type end = string.find_first_of(delimiters, begin);
	while (begin != std::string::npos || end != std::string::npos)
	{
		substrings.push_back(string.substr(begin, end - begin));
		begin = string.find_first_not_of(delimiters, end);
		end = string.find_first_of(delimiters, begin);
	}
	return substrings;
}

void string::Trim(std::string& string)
{
	std::string::size_type begin = string.find_first_not_of(whitespace, 0);
	std::string::size_type end = string.find_last_not_of(whitespace) + 1;
	if (begin != std::string::npos && end != std::string::npos)
	{
		string = string.substr(begin, end - begin);
	}
	else
	{
		string.clear();
	}
}

std::string string::ToLower(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), std::tolower);
	return string;
}

std::string string::ToUpper(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), std::toupper);
	return string;
}
