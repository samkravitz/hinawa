#include "html_image_element.h"

#include "web/resource.h"

void HtmlImageElement::add_attribute(std::string name, std::string value)
{
	if (name == "src")
	{
		load(Url{ value }, [this](const auto &data) {
			m_image.loadFromMemory(data.data(), data.size());
		});
	}

	Element::add_attribute(name, value);
}

float HtmlImageElement::width() const
{
	return m_image.getSize().x;
}

float HtmlImageElement::height() const
{
	return m_image.getSize().y;
}
