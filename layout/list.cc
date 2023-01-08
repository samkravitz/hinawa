#include "list.h"

namespace layout
{
void ListItemMarker::layout(Box container)
{
	m_dimensions = container;
	m_dimensions.content.width = 8;
	m_dimensions.content.height = 8;
}

ListItem::ListItem(css::StyledNode *style) :
    Block(style)
{
	prepend_child(std::make_shared<ListItemMarker>());
}

void ListItem::layout(Box container)
{
	Node *marker = children[0].get();
	m_dimensions.content.x += marker->dimensions().margin_box().x;
	m_dimensions.content.y += marker->dimensions().margin_box().y;
	Block::layout(container);
}
}
