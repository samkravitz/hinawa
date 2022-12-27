#pragma once

#include "element.h"

#include <SFML/Graphics.hpp>

class HtmlImageElement final : public Element
{
public:
	HtmlImageElement(std::string tag) :
	    Element(tag)
	{ }

	void add_attribute(std::string, std::string) override;

private:
	sf::Image image;
};
