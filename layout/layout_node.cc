#include "layout_node.h"

namespace layout
{
LayoutNode::LayoutNode(std::shared_ptr<css::StyledNode> node) :
	m_node(node)
{
	auto f = [this](std::shared_ptr<css::StyledNode> node)
	{
		children.push_back(std::make_shared<LayoutNode>(LayoutNode(node)));
	};

	node->for_each_child(f);
}
}
