#pragma once

#include "document/html_canvas_element.h"
#include "gfx/color.h"

class CanvasRenderingContext2D
{
public:
	CanvasRenderingContext2D(HtmlCanvasElement *canvas_element) :
	    m_canvas_element(canvas_element)
	{ }

	void set_fill_style(std::string);
	std::string fill_style() const;

	void fill_rect(int, int, int, int);

private:
	HtmlCanvasElement *m_canvas_element = nullptr;

	Color m_fill_style;
};
