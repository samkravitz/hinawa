#include "painter.h"

#include "SkCanvas.h"
#include "SkRect.h"
#include "SkString.h"
#include "SkTextBlob.h"

namespace browser
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
}