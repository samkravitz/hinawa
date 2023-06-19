#include "element_factory.h"

std::shared_ptr<Element> create_element(Document &document, const std::string &name)
{
	if (name == "img")
		return std::make_shared<HtmlImageElement>(document, name);

	return std::make_shared<Element>(document, name);
}

std::shared_ptr<Element> create_element(const std::string &name)
{
	return std::make_shared<Element>(name);
}
