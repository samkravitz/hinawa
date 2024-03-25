#pragma once

#include "color.h"
#include "layout/rect.h"
#include "point.h"

#include "SkImage.h"

class SkCanvas;
class SkFont;

namespace gfx
{
class Painter
{
public:
	Painter() :
	    Painter(nullptr, 0, 0)
	{ }

	Painter(SkCanvas *, int, int);

	void fill_rect(const layout::Rect &, const Color &);
	void stroke_rect(const layout::Rect &, const Color &);
	void fill_rect(const Color &);
	void draw_text(const std::string &, const SkFont &, int, int, const Color &color = Color::BLACK);
	void draw_image(const sk_sp<SkImage> &, int, int);
	void draw_circle(const Point &, float, const Color &color = Color::BLACK);
	void scale(float, float);
	void translate(float, float);

private:
	SkCanvas *canvas;
	int width = 0;
	int height = 0;
};
}