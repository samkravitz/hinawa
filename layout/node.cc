#include "node.h"

#include <iostream>

#include "block.h"
#include "css/value.h"
#include "inline.h"
#include "text.h"

namespace layout
{
std::shared_ptr<Node> build_layout_tree(css::StyledNode *styled_node)
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
	    [node, &contains_inline_children, &contains_block_children](auto *styled_child)
	    {
		    auto child = build_layout_tree(styled_child);
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

Node::Node(css::StyledNode *node) :
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
			anonymous_box->m_parent = this;
			goto loop;
		}
	}
}

std::string Node::tag_name() const
{
	return m_node->node()->element_name();
}

void print_tree_with_lines(Node *root)
{
	static int indent = 0;
	for (int i = 0; i < indent * 2; i++)
		std::cout << " ";

	std::cout << root->to_string() << "\n";
	if (root->is_block())
	{
		auto *block = static_cast<Block *>(root);
		int saved_indent = indent;
		indent++;
		for (uint i = 0; i < block->lines.size(); i++)
		{
			auto line = block->lines[i];
			for (int i = 0; i < indent * 2; i++)
				std::cout << " ";
			std::cout << "line " << i << ": "
			          << "(" << line.x << ", " << line.y << ") "
			          << "[" << line.width << "x" << line.height << "]\n";

			indent++;
			for (uint j = 0; j < line.fragments.size(); j++)
			{
				auto frag = line.fragments[j];
				for (int i = 0; i < indent * 2; i++)
					std::cout << " ";
				std::cout << "frag " << j << ": "
				          << " offset: " << frag.offset << " len: " << frag.len << " str: " << frag.str << "\n";
			}
			indent--;
		}
		indent = saved_indent;
	}

	root->for_each_child([](auto *child)
	{
		indent++;
		print_tree_with_lines(child);
		indent--;
	});
}
}
