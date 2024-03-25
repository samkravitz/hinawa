#include "painter.h"

#include "SkCanvas.h"
#include "SkFont.h"
#include "SkRect.h"
#include "SkString.h"

namespace gfx
{
Painter::Painter(SkCanvas *canvas, int width, int height) :
    canvas(canvas),
    width(width),
    height(height)
{ }

void Painter::fill_rect(const layout::Rect &rect, const Color &color)
{
	SkPaint paint;
	paint.setStyle(SkPaint::kFill_Style);
	paint.setAntiAlias(true);
	paint.setColor(color.to_u32());

	SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
	canvas->drawRect(sk_rect, paint);
}

void Painter::stroke_rect(const layout::Rect &rect, const Color &color)
{
	SkPaint paint;
	paint.setStyle(SkPaint::kStroke_Style);
	paint.setAntiAlias(true);
	paint.setColor(color.to_u32());

	SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
	canvas->drawRect(sk_rect, paint);
}

void Painter::fill_rect(const Color &color)
{
	fill_rect({0, 0, width, height}, color);
}

void Painter::draw_text(const std::string &str, const SkFont &font, int x, int y, const Color &color)
{
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(color.to_u32());

	canvas->drawString(SkString(str), x, y + font.getSize(), font, paint);
}

void Painter::draw_image(const sk_sp<SkImage> &image, int x, int y)
{
	canvas->drawImage(image, x, y);
}

void Painter::draw_circle(const Point &at, float r, const Color &color)
{
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(color.to_u32());

	canvas->drawCircle(at.x, at.y, r, paint);
}

void Painter::scale(float sx, float sy)
{
	canvas->scale(sx, sy);
}

void Painter::translate(float dx, float dy)
{
	canvas->translate(dx, dy);
}

void Painter::rotate(float degrees)
{
	canvas->rotate(degrees);
}
}