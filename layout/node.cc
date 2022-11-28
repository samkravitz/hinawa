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

	styled_node->for_each_child(
	    [node, &contains_inline_children, &contains_block_children](std::shared_ptr<css::StyledNode> styled_child)
	    {
		    auto child = build_tree(styled_child);
		    node->add_child(child);

		    if (child->is_inline())
			    contains_inline_children = true;
		    else
			    contains_block_children = true;
	    });

	if (contains_inline_children && contains_block_children)
		node->insert_anonymous_container();

	if (contains_block_children)
		node->set_block_format_context();

	return node;
}

Node::Node() :
    m_node(nullptr)
{ }

Node::Node(std::shared_ptr<css::StyledNode> node) :
    m_node(node)
{ }

void Node::insert_anonymous_container()
{
loop:
	for (unsigned int i = 0; i < children.size(); i++)
	{
		auto child = children[i];
		if (child->is_inline())
		{
			auto first_inline_index = i;
			auto last_inline_index = i;
			auto anonymous_box = std::make_shared<Block>();
			while (last_inline_index < children.size() && (children[last_inline_index]->is_inline()))
			{
				anonymous_box->add_child(children[last_inline_index]);
				last_inline_index++;
			}

			children.erase(children.begin() + first_inline_index, children.begin() + last_inline_index);
			children.insert(children.begin() + first_inline_index, anonymous_box);
			goto loop;
		}
	}
}

std::string Node::tag_name() const
{
	return m_node->node()->element_name();
}
}