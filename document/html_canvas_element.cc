#include "html_canvas_element.h"

#include <cassert>

#include "canvas_rendering_context_2d.h"

void HtmlCanvasElement::add_attribute(std::string name, std::string value)
{
	if (name == "width")
	{
		m_width = std::stof(value);
	}

	if (name == "height")
	{
		m_height = std::stof(value);
	}

	Element::add_attribute(name, value);

	auto info = SkImageInfo::MakeN32Premul(width(), height());
	auto row_bytes = info.minRowBytes();
	auto size = info.computeByteSize(row_bytes);
	m_pixels.reserve(size);
	m_surface = SkSurfaces::WrapPixels(info, &m_pixels[0], row_bytes);
}

float HtmlCanvasElement::width() const
{
	return m_width;
}

float HtmlCanvasElement::height() const
{
	return m_height;
}

CanvasRenderingContext2D *HtmlCanvasElement::get_context(std::string type)
{
	assert(type == "2d");
	if (!m_context)
		m_context = new CanvasRenderingContext2D(this);

	return m_context;
}
