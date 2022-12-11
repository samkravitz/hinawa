#include "layout_node.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include "../css/value.h"
#include "../document/text.h"
#include "../render/window.h"

#include <SFML/Graphics.hpp>

extern sf::Font font;

namespace layout
{
// constructor for an anonymous box
// anonymous boxes by definition do not point to a DOM node,
// so we set it to null
LayoutNode::LayoutNode() :
    m_node(nullptr)
{
	m_box_type = ANONYMOUS;
}

LayoutNode::LayoutNode(std::shared_ptr<css::StyledNode> node) :
    m_node(node)
{
	switch (node->display())
	{
		case css::Display::Block: m_box_type = BLOCK; break;
		case css::Display::Inline: m_box_type = INLINE; break;
		default: std::cerr << "Found display: none!\n";
	}

	bool contains_inline_children = false;
	bool contains_block_children = false;

	node->for_each_child(
	    [this, &contains_inline_children, &contains_block_children](std::shared_ptr<css::StyledNode> child)
	    {
		    switch (child->display())
		    {
			    case css::Display::Inline:
			    {
				    contains_inline_children = true;
				    auto inline_child = std::make_shared<LayoutNode>(LayoutNode(child));
				    if (inline_child->children.size() == 0)
				    {
					    add_child(inline_child);
				    }

				    else
				    {
					    for (auto x : inline_child->children)
						    add_child(x);
				    }
				    break;
			    }

			    case css::Display::Block:
			    {
				    contains_block_children = true;
				    add_child(std::make_shared<LayoutNode>(LayoutNode(child)));
				    break;
			    }

			    case css::Display::None: std::cout << "display_none\n";
		    }
	    });

	if (contains_block_children && contains_inline_children)
	{
	loop:
		for (int i = 0; i < children.size(); i++)
		{
			auto child = children[i];
			if (child->box_type() == INLINE)
			{
				int first_inline_index = i;
				int last_inline_index = i;
				auto anonymous_box = std::make_shared<LayoutNode>(LayoutNode());
				while ((last_inline_index < children.size()) && (children[last_inline_index]->box_type() == INLINE))
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

	if (contains_block_children)
		m_format_context = FormatContext::Block;
}

void LayoutNode::layout(Box container)
{
	reset();
	switch (m_box_type)
	{
		case BLOCK:
			if (m_format_context == FormatContext::Block)
				layout_block(container);
			else
				layout_inline(container);
			break;
		case ANONYMOUS: layout_inline(container); break;
	}
}

void LayoutNode::layout_block(Box container)
{
	calculate_block_width(container);
	calculate_block_position(container);

	for (auto child : children)
	{
		child->layout(m_dimensions);
		m_dimensions.content.height += child->dimensions().margin_box().height;
	}

	calculate_block_height(container);
}

void LayoutNode::layout_inline(Box container)
{
	m_dimensions = container;
	m_dimensions.content.y = container.content.height;
	Point offset = Point{ m_dimensions.content.x, m_dimensions.content.y };
	lines.clear();

	preorder([this, &offset](std::shared_ptr<LayoutNode>(child))
	{
		offset = child->split_into_lines(m_dimensions, offset, this);
	});

	m_dimensions.content.height = 0;
	for (auto const &line : lines)
		m_dimensions.content.height += line.height;
}

Point LayoutNode::split_into_lines(const Box &container_start, const Point &offset, LayoutNode *containing_block)
{
	auto *font_size = dynamic_cast<css::Length *>(m_node->lookup("font-size"));

	auto text_element = std::dynamic_pointer_cast<Text>(m_node->node());
	auto str = text_element->trim();
	auto px = font_size->to_px();
	const int max_width = container_start.content.width;
	int start_y = offset.y;
	int start_x = offset.x;
	int current_x = start_x;
	int current_y = start_y;
	float max_height = 0;

	if (containing_block->lines.empty())
		containing_block->lines.push_back(Line(current_x, current_y));

	auto space = sf::Text(" ", font, px);
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(px);

	std::istringstream ss(str);
	std::string word;
	while (std::getline(ss, word, ' '))
	{
		LineItem item;
		item.str = word;
		text.setString(word);
		max_height = std::max(max_height, text.getLocalBounds().height);
		containing_block->lines.back().height = max_height;

		int len = text.getLocalBounds().width;

		// item would overflow the max allowed width, so make a new line
		if (current_x + len > max_width)
		{
			current_x = container_start.content.x;
			current_y += max_height;
			containing_block->lines.push_back(Line(current_x, current_y));
			containing_block->lines.back().height = max_height;
			containing_block->lines.back().width = len;
			max_height = 0;
		}

		// item can fit on the same line
		else
		{
			containing_block->lines.back().width += len;
		}

		current_x += len;
		current_x += space.getLocalBounds().width;

		item.offset = current_x - container_start.content.x - len;
		item.len = len;
		item.styled_node = node().get();
		containing_block->lines.back().items.push_back(item);
	}

	int height = 0;
	for (auto const &line : containing_block->lines)
		height += line.height;

	return Point{ current_x, current_y };
}

void LayoutNode::calculate_block_width(Box container)
{
	auto zero = css::Length{};

	auto *margin_left = m_node->lookup("margin-left", "margin", &zero);
	auto *margin_right = m_node->lookup("margin-right", "margin", &zero);

	auto *border_left = m_node->lookup("border-left", "border", &zero);
	auto *border_right = m_node->lookup("border-right", "border", &zero);

	auto *padding_left = m_node->lookup("padding-left", "padding", &zero);
	auto *padding_right = m_node->lookup("padding-right", "padding", &zero);

	// total width in px
	int total = 0;
	total += margin_left->to_px();
	total += margin_right->to_px();
	total += border_left->to_px();
	total += border_right->to_px();
	total += padding_left->to_px();
	total += padding_right->to_px();

	m_dimensions.content.width = container.content.width - total;

	m_dimensions.padding.left = padding_left->to_px();
	m_dimensions.padding.right = padding_right->to_px();

	m_dimensions.border.left = border_left->to_px();
	m_dimensions.border.right = border_right->to_px();

	m_dimensions.margin.left = margin_left->to_px();
	m_dimensions.margin.right = margin_right->to_px();
}

void LayoutNode::calculate_block_position(Box container)
{
	auto zero = css::Length{};

	m_dimensions.margin.top = m_node->lookup("margin-top", "margin", &zero)->to_px();
	m_dimensions.margin.bottom = m_node->lookup("margin-bottom", "margin", &zero)->to_px();

	m_dimensions.border.top = m_node->lookup("border-top", "border", &zero)->to_px();
	m_dimensions.border.bottom = m_node->lookup("border-bottom", "border", &zero)->to_px();

	m_dimensions.padding.top = m_node->lookup("padding-top", "padding", &zero)->to_px();
	m_dimensions.padding.bottom = m_node->lookup("padding-bottom", "padding", &zero)->to_px();

	m_dimensions.content.x =
	    container.content.x + m_dimensions.margin.left + m_dimensions.border.left + m_dimensions.padding.left;
	m_dimensions.content.y = container.content.height + container.content.y + m_dimensions.margin.top +
	                         m_dimensions.border.top + m_dimensions.padding.top;
}

void LayoutNode::calculate_block_height(Box container) { }

void LayoutNode::reset()
{
	m_dimensions = Box();
	lines.clear();
}

std::string LayoutNode::to_string() const
{
	std::stringstream ss;

	switch (m_box_type)
	{
		case BLOCK: ss << "BLOCK"; break;
		case ANONYMOUS: ss << "ANONYMOUS"; break;
		case INLINE: ss << "INLINE"; break;
	}

	ss << " ";
	ss << m_dimensions.to_string();
	return ss.str();
}
}
