#pragma once

#include "layout/rect.h"
#include "util/color.h"

class SkCanvas;

namespace browser
{
class Painter
{
public:
	Painter(SkCanvas *, int, int);

	void fill_rect(const layout::Rect &, const Color &);
	void fill_rect(const Color &);

private:
	SkCanvas *canvas;
	int width = 0;
	int height = 0;
};
}