#include "layout_node.h"

#include <iostream>
#include <memory>

#include "../css/value.h"

namespace layout
{
LayoutNode::LayoutNode(std::shared_ptr<css::StyledNode> node) :
    m_node(node)
{
	auto f = [this](std::shared_ptr<css::StyledNode> node)
	{ children.push_back(std::make_shared<LayoutNode>(LayoutNode(node))); };

	node->for_each_child(f);
}

void LayoutNode::calculate_layout(Box container)
{
	calculate_width(container);
	calculate_position(container);

	for (auto child : children)
	{
		child->calculate_layout(m_dimensions);

		auto margins = child->margin();
		m_dimensions.content.height = margins.top + margins.bottom;
	}

	calculate_height(container);
}

void LayoutNode::calculate_width(Box container)
{
	auto len = css::Length{ 12.0, css::Length::PX };
	auto zero = css::Length{};

	auto *margin_left = m_node->lookup("margin-left", &zero);
	auto *margin_right = m_node->lookup("margin-right", &zero);

	auto *border_left = m_node->lookup("border-left", &zero);
	auto *border_right = m_node->lookup("border-right", &zero);

	auto *padding_left = m_node->lookup("padding-left", &len);
	auto *padding_right = m_node->lookup("padding-right", &len);

	// total width in px
	int total = 0;
	total += margin_left->to_px();
	total += margin_right->to_px();
	total += border_left->to_px();
	total += border_right->to_px();
	total += padding_left->to_px();
	total += padding_right->to_px();

	m_dimensions.content.width = total;

	m_dimensions.padding.left = padding_left->to_px();
	m_dimensions.padding.right = padding_right->to_px();

	m_dimensions.border.left = border_left->to_px();
	m_dimensions.border.right = border_right->to_px();

	m_dimensions.margin.left = margin_left->to_px();
	m_dimensions.margin.right = margin_right->to_px();
}

void LayoutNode::calculate_position(Box container)
{
	auto len = css::Length{ 12.0, css::Length::PX };
	auto zero = css::Length{};

	m_dimensions.margin.top = m_node->lookup("margin-top", &zero)->to_px();
	m_dimensions.margin.bottom = m_node->lookup("margin-bottom", &zero)->to_px();

	m_dimensions.border.top = m_node->lookup("border-top", &zero)->to_px();
	m_dimensions.border.bottom = m_node->lookup("border-bottom", &zero)->to_px();

	m_dimensions.padding.top = m_node->lookup("padding-top", &len)->to_px();
	m_dimensions.padding.bottom = m_node->lookup("padding-bottom", &len)->to_px();

	m_dimensions.content.x =
	    container.content.x + m_dimensions.margin.left + m_dimensions.border.left + m_dimensions.padding.left;
	m_dimensions.content.y = container.content.height + container.content.y + m_dimensions.margin.top +
	                         m_dimensions.border.top + m_dimensions.padding.top;
}

void LayoutNode::calculate_height(Box container) { }
}
