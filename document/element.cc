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
	std::cout << "HTML " << m_tag << " Element" << "\n";
	for (auto child : children)
		child->print(depth + 1);
}
