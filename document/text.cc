#include "text.h"

#include <iostream>

Text::Text(std::string text) :
	text(text)
{ }

Text::Text(char c)
{
	text = c;
}

void Text::append(char c)
{
	text += c;
}

void Text::print(int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";

	std::cout << "Text: " << text << "\n";

	for (auto child : children)
		child->print(depth + 1);
}
