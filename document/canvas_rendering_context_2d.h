#pragma once

#include "document/html_canvas_element.h"
#include "gfx/color.h"
#include "gfx/painter.h"

class CanvasRenderingContext2D
{
public:
	CanvasRenderingContext2D(HtmlCanvasElement *canvas_element) :
	    m_canvas_element(canvas_element)
	{
		m_painter = gfx::Painter(&canvas_element->canvas(), canvas_element->width(), canvas_element->height());
	}

	void set_fill_style(std::string);
	std::string fill_style() const;

	void fill_rect(int, int, int, int);

private:
	HtmlCanvasElement *m_canvas_element = nullptr;
	gfx::Painter m_painter;
	std::string m_fill_style = "";
};
