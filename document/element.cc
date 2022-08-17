#include "element.h"

#include <iostream>

Element::Element(std::string tag)
	: m_tag(tag)
{
	
}

void Element::print(int depth) const
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";

	std::cout << "HTML " << m_tag << " Element ";
	for (auto attribute : attrs)
	{
		std::cout << attribute.first;
		std::cout << "=";
		std::cout << "\"";
		std::cout << attribute.second;
		std::cout << "\"";
		std::cout << " ";
	}

	std::cout << "\n";

	for (auto child : children)
		child->print(depth + 1);
}

void Element::add_attribute(std::string name, std::string value)
{
	attrs[name] = value;
}

bool Element::has_attribute(std::string name) const
{
	return attrs.find(name) != attrs.end();
}

std::string Element::get_attribute(std::string name)
{
	return attrs[name];
}
