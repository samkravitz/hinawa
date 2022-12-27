#include "image.h"

namespace layout
{
Image::Image(css::StyledNode *node) :
    Node(node)
{ }

void Image::layout(Box container)
{
	m_dimensions = container;
	m_dimensions.content.width = image_element()->width();
	m_dimensions.content.height = image_element()->height();
}
}
