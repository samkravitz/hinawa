#include "text.h"

#include <algorithm>
#include <cctype>
#include <iostream>

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

void Text::print(int depth) const
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";

	std::cout << "Text: " << m_text << "\n";

	for (auto child : children)
		child->print(depth + 1);
}

bool Text::whitespace_only() const
{
	return std::all_of(m_text.begin(), m_text.end(), [](char c) { return std::isspace(c); });
}
