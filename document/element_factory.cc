#include "element_factory.h"

std::shared_ptr<Element> create_element(const std::string &name)
{
	if (name == "img")
		return std::make_shared<HtmlImageElement>(name);

	return std::make_shared<Element>(name);
}
