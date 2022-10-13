#include "node.h"

#include <iostream>

#include "../css/value.h"
#include "block.h"
#include "inline.h"
#include "text.h"

namespace layout
{
std::shared_ptr<Node> build_tree(std::shared_ptr<css::StyledNode> styled_node)
{
	std::shared_ptr<Node> node = nullptr;

	switch (styled_node->display())
	{
		case css::Display::Block: node = std::make_shared<Block>(styled_node); break;
		case css::Display::Inline:
			if (styled_node->node()->type() == NodeType::Text)
				node = std::make_shared<Text>(styled_node);
			else
				node = std::make_shared<Inline>(styled_node);
			break;
		default:
			std::cerr << "Found display: none!\n";
			return nullptr;
	}

	bool contains_inline_children = false;
	bool contains_block_children = false;

	styled_node->for_each_child([node, &contains_inline_children, &contains_block_children](
	                                std::shared_ptr<css::StyledNode> child) { node->add_child(build_tree(child)); });

	return node;
}

Node::Node() :
    m_node(nullptr)
{ }

Node::Node(std::shared_ptr<css::StyledNode> node) :
    m_node(node)
{ }
}
