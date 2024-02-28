#pragma once

#include "block.h"

#include "document/html_canvas_element.h"

namespace layout
{
class Canvas final : public Block
{
public:
	Canvas(css::StyledNode *);
	Canvas() = delete;

	void layout(Box) override;
	HtmlCanvasElement *canvas_element() const { return static_cast<HtmlCanvasElement *>(m_style->node()); }
	bool is_canvas() const override { return true; }

	void render(gfx::Painter &) const override;
};
}
