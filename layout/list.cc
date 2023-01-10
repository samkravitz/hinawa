#include "list.h"

namespace layout
{
void ListItemMarker::layout(Box container)
{
	m_dimensions = {};
	m_dimensions.content.x = container.margin_box().x;
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
	Block::layout(container);

	auto *marker = children[0].get();
	marker->dimensions().content.y = m_dimensions.margin_box().y + m_dimensions.margin_box().height / 2;
}
}
