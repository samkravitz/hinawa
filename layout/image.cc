#include "image.h"

namespace layout
{
Image::Image(css::StyledNode* node) :
    Block(node)
{ }

void Image::layout(Box container)
{
	m_dimensions.content.width = image_element()->width();
	Block::layout(container);
	m_dimensions.content.height = image_element()->height();
}
}
