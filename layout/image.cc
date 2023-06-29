#include "image.h"
#include "SkImage.h"

namespace layout
{
Image::Image(css::StyledNode *node) :
    Block(node)
{ }

void Image::layout(Box container)
{
	m_dimensions.content.width = image_element()->width();
	Block::layout(container);
	m_dimensions.content.height = image_element()->height();
}

void Image::render(gfx::Painter &painter) const
{
	auto image = image_element()->bitmap().asImage();
	painter.draw_image(image, m_dimensions.content.x, m_dimensions.content.y);
}
}
