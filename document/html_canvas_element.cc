#include "html_canvas_element.h"

#include <cassert>

#include "canvas_rendering_context_2d.h"

float HtmlCanvasElement::width() const
{
	return m_bitmap.width();
}

float HtmlCanvasElement::height() const
{
	return m_bitmap.height();
}

CanvasRenderingContext2D *HtmlCanvasElement::get_context(std::string type)
{
	assert(type == "2d");
	if (!m_context)
		m_context = new CanvasRenderingContext2D(this);

	return m_context;
}
