#include "element.h"

Element::Element(std::string tag)
	: m_tag(tag)
{
	
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

std::string Element::to_string() const
{
	return "HTML " + m_tag + " Element";
}
