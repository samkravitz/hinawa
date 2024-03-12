#include "canvas.h"

#include <cassert>

#include "SkBitmap.h"
#include "SkImage.h"
#include "SkImageInfo.h"

namespace layout
{
Canvas::Canvas(css::StyledNode *node) :
    Block(node)
{ }

void Canvas::layout(Box container)
{
	m_dimensions.content.width = canvas_element()->width();
	Block::layout(container);
	m_dimensions.content.height = canvas_element()->height();
}

void Canvas::render(gfx::Painter &painter) const
{
	SkBitmap bitmap;
	auto info = SkImageInfo::MakeN32Premul(canvas_element()->width(), canvas_element()->height());
	bitmap.setInfo(info);
	bitmap.allocPixels(info);

	if (!canvas_element()->canvas().readPixels(bitmap, 0, 0))
		assert(!"Failed to copy pixels from HTML canvas element!");

	auto canvas = bitmap.asImage();
	painter.draw_image(canvas, m_dimensions.content.x, m_dimensions.content.y);
}
}
