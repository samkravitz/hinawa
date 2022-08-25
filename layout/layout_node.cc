#include "layout_node.h"

#include <iostream>
#include <memory>

#include "../css/value.h"
#include "../render/window.h"

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

	node->for_each_child(
	    [this](std::shared_ptr<css::StyledNode> child)
	    {
		    switch (child->display())
		    {
			    case css::Display::Inline:
			    {
				    auto last_layout_child = last_child();
				    if (!last_layout_child || last_layout_child->box_type() == INLINE)
				    {
					    add_child(std::make_shared<LayoutNode>(LayoutNode(child)));
				    }

				    else
				    {
					    auto anonymous_box = std::make_shared<LayoutNode>(LayoutNode());
					    anonymous_box->add_child(std::make_shared<LayoutNode>(LayoutNode(child)));
					    add_child(anonymous_box);
				    }
				    break;
			    }

			    case css::Display::Block:
			    {
				    auto last_layout_child = last_child();
				    if (!last_layout_child || last_layout_child->box_type() == BLOCK)
				    {
					    add_child(std::make_shared<LayoutNode>(LayoutNode(child)));
				    }

				    else
				    {
					    auto anonymous_box = std::make_shared<LayoutNode>(LayoutNode());
					    anonymous_box->add_child(last_layout_child);
					    children.pop_back();
					    add_child(anonymous_box);
					    add_child(std::make_shared<LayoutNode>(LayoutNode(child)));
				    }
					break;
			    }

			    case css::Display::None:
					std::cout << "display_none\n";
		    }
	    });
}

void LayoutNode::layout(Box container)
{
	switch (m_box_type)
	{
		case BLOCK:
		case ANONYMOUS: layout_block(container); break;
		case INLINE:
		{
			auto *font_size = dynamic_cast<css::Length*>(m_node->lookup("font-size"));
			m_dimensions.content.height = font_size->to_px();
			m_dimensions.content.width = container.content.width;
			m_dimensions.content.x = container.content.x;
			m_dimensions.content.y = container.content.y;

			for (auto child : children)
				child->layout(m_dimensions);
			break;
		}
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
