#pragma once

#include "node.h"

#include "document/html_image_element.h"

namespace layout
{
class Image : public Node
{
public:
	Image(css::StyledNode *);
	Image() = delete;

	void layout(Box) override;
	HtmlImageElement *image_element() const { return static_cast<HtmlImageElement*>(m_style->node()); }
	bool is_image() const override { return true; }

	std::string to_string() const { return "<Image>"; }
};
}
