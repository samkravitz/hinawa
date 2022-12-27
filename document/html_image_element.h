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
	float width() const;
	float height() const;
	sf::Image image() const { return m_image; }

private:
	sf::Image m_image;
};
