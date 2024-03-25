#include "canvas_rendering_context_2d.h"

#include "gfx/color.h"
#include "layout/rect.h"

void CanvasRenderingContext2D::set_fill_style(std::string fill_style)
{
	m_fill_style = fill_style;
}

std::string CanvasRenderingContext2D::fill_style() const
{
	return m_fill_style;
}

void CanvasRenderingContext2D::set_stroke_style(std::string stroke_style)
{
	m_stroke_style = stroke_style;
}

std::string CanvasRenderingContext2D::stroke_style() const
{
	return m_stroke_style;
}

void CanvasRenderingContext2D::fill_rect(int x, int y, int width, int height)
{
	auto rect = layout::Rect{x, y, width, height};
	auto color = Color::from_string(fill_style());
	m_painter.fill_rect(rect, color);
}

void CanvasRenderingContext2D::stroke_rect(int x, int y, int width, int height)
{
	auto rect = layout::Rect{x, y, width, height};
	auto color = Color::from_string(stroke_style());
	m_painter.stroke_rect(rect, color);
}

void CanvasRenderingContext2D::scale(float sx, float sy)
{
	m_painter.scale(sx, sy);
}

void CanvasRenderingContext2D::translate(float dx, float dy)
{
	m_painter.translate(dx, dy);
}

void CanvasRenderingContext2D::rotate(float degrees)
{
	m_painter.rotate(degrees);
}
