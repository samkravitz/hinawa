#include "text.h"

#include <algorithm>
#include <cctype>

static const std::string WHITESPACE = " \n\r\t\f\v";

auto ltrim(const std::string &s) -> std::string
{
	auto start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
};

auto rtrim(const std::string &s) -> std::string
{
	auto end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
};

Text::Text(std::string text) :
    m_text(text)
{ }

Text::Text(char c)
{
	m_text = c;
}

void Text::append(char c)
{
	m_text += c;
}

bool Text::whitespace_only() const
{
	return std::all_of(m_text.begin(), m_text.end(), [](char c) { return std::isspace(c); });
}

std::string Text::trim()
{
	// remove all whitespace from beginning and end of text
	auto whitespace_trimmed = rtrim(ltrim(m_text));

	// replace all newlines with spaces
	std::string::size_type pos = 0;    // Must initialize
	while ((pos = whitespace_trimmed.find("\n", pos)) != std::string::npos)
	{
		whitespace_trimmed.replace(pos, 1, " ");
	}

	// condense all adjacent whitespace into a single space
	// "hello     world" => "hello world"
	char prev = ' ';
	auto iter = std::remove_if(whitespace_trimmed.begin(),
	                           whitespace_trimmed.end(),
	                           [&](char c) -> bool
	                           {
		                           bool ret = false;
		                           if (std::isspace(c) && std::isspace(prev))
			                           ret = true;

		                           prev = c;
		                           return ret;
	                           });
	whitespace_trimmed.erase(iter, whitespace_trimmed.end());

	whitespace_trimmed.erase(iter, whitespace_trimmed.end());
	return whitespace_trimmed;
}

std::string Text::to_string() const
{
	return "Text: " + m_text;
}
