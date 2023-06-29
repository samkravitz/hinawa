#pragma once

#include "layout/rect.h"
#include "util/color.h"

#include "SkImage.h"

class SkCanvas;
class SkFont;

namespace browser
{
class Painter
{
public:
	Painter(SkCanvas *, int, int);

	void fill_rect(const layout::Rect &, const Color &);
	void fill_rect(const Color &);
	void draw_text(const std::string &, const SkFont &, int, int, const Color &color = Color::BLACK);
	void draw_image(const sk_sp<SkImage> &, int, int);
	void draw_circle(const Point &, float, const Color &color = Color::BLACK);

private:
	SkCanvas *canvas;
	int width = 0;
	int height = 0;
};
}