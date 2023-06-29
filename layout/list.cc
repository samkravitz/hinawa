#include "list.h"
#include "layout/block.h"

#include <cassert>

namespace layout
{
void ListItemMarker::layout(Box container)
{
	m_dimensions = {};
	m_dimensions.content.x = container.margin_box().x;
	m_dimensions.content.width = 8;
	m_dimensions.content.height = 8;

	assert(parent()->is_block());
	// auto* block_parent = static_cast<Block*>(parent());
	// auto &lines = block_parent->lines;

	Line line = {container.content.x, container.content.y};
	line.width = 8;
	line.height = 8;

	LineFragment frag = {};
	frag.len = m_dimensions.content.width = 8;
	line.fragments.push_back(frag);
	// lines.push_back(line);
}

void ListItemMarker::render(gfx::Painter &painter) const
{
	painter.draw_circle({m_dimensions.content.x, m_dimensions.content.y}, m_dimensions.content.width / 2.0f);
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
