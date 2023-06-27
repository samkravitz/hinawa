#include "painter.h"

#include "SkCanvas.h"
#include "SkRect.h"

namespace browser
{
Painter::Painter(SkCanvas *canvas, int width, int height) :
    canvas(canvas),
	width(width),
	height(height)
{ }

void Painter::fill_rect(const layout::Rect &rect, const Color &color)
{
	auto to_skia_color = [](const Color &c) -> u32 {
		u32 a = c.a;
		u32 r = c.r;
		u32 g = c.g;
		u32 b = c.b;

		return (a << 24) | (r << 16) | (g << 8) | b;
	};

	SkPaint paint;
	paint.setStyle(SkPaint::kFill_Style);
	paint.setAntiAlias(true);
	paint.setColor(to_skia_color(color));

	SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
	canvas->drawRect(sk_rect, paint);
}

void Painter::fill_rect(const Color &color)
{
	fill_rect({0, 0, width, height}, color);
}
}