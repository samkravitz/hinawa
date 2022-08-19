#include "text.h"

#include <algorithm>
#include <cctype>

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

std::string Text::to_string() const
{
	return "Text: " + m_text;
}
