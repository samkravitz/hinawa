#include "element.h"

#include <sstream>

Element::Element(Document &document, std::string tag) :
    Node(document)
{
	m_tag = tag;
}

Element::Element(std::string tag) :
    m_tag(tag)
{ }

void Element::add_attribute(std::string name, std::string value)
{
	attrs[name] = value;
}

bool Element::has_attribute(const std::string &name) const
{
	return attrs.find(name) != attrs.end();
}

std::string Element::get_attribute(const std::string &name) const
{
	return attrs.at(name);
}

bool Element::has_class(const std::string &name) const
{
	if (!has_attribute("class"))
		return false;
	
	auto classes = get_attribute("class");
	std::istringstream ss(classes);
	std::string classname;
	while (std::getline(ss, classname, ' '))
	{
		if (classname == name)
			return true;
	}

	return false;
}

std::string Element::to_string() const
{
	return "HTML " + m_tag + " Element";
}
