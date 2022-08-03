#include "element.h"

#include <iostream>

Element::Element(std::string tag)
	: tag(tag)
{
	
}

void Element::print(int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";
	std::cout << "HTML " << tag << " Element" << "\n";
	for (auto child : children)
		child->print(depth + 1);
}
