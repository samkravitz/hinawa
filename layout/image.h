#pragma once

#include "block.h"

#include "document/html_image_element.h"

namespace layout
{
class Image : public Block
{
public:
	Image(css::StyledNode*);
	Image() = delete;

	void layout(Box) override;
	HtmlImageElement* image_element() const { return static_cast<HtmlImageElement*>(m_style->node()); }
	bool is_image() const override { return true; }
};
}
