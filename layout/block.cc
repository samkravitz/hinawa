#include "block.h"

namespace layout
{
Block::Block() :
    Node(nullptr)
{
	m_anonymous = true;
}

Block::Block(css::StyledNode *node) :
    Node(node)
{
	m_anonymous = false;
}

void Block::layout(Box container)
{
	m_dimensions = {};
	lines = {};

	calculate_width(container);
	calculate_position(container);

	if (m_inline_format_context)
	{
		for (auto child : children)
			child->layout(m_dimensions);

		for (const auto &line : lines)
			m_dimensions.content.height += line.height;
	}

	else
	{
		for (auto child : children)
		{
			child->layout(m_dimensions);
			m_dimensions.content.height += child->dimensions().margin_box().height;
		}
	}

	calculate_height(container);
}

void Block::calculate_width(Box container)
{
	if (m_anonymous)
		m_style = parent()->style();

	auto *margin_left = m_style->property("margin-left");
	auto *margin_right = m_style->property("margin-right");

	auto *border_left = m_style->property("border-left");
	auto *border_right = m_style->property("border-right");

	auto *padding_left = m_style->property("padding-left");
	auto *padding_right = m_style->property("padding-right");

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

void Block::calculate_position(Box container)
{
	if (m_anonymous)
		m_style = parent()->style();

	m_dimensions.margin.top = m_style->property("margin-top")->to_px();
	m_dimensions.margin.bottom = m_style->property("margin-bottom")->to_px();

	m_dimensions.border.top = m_style->property("border-top")->to_px();
	m_dimensions.border.bottom = m_style->property("border-bottom")->to_px();

	m_dimensions.padding.top = m_style->property("padding-top")->to_px();
	m_dimensions.padding.bottom = m_style->property("padding-bottom")->to_px();

	m_dimensions.content.x =
	    container.content.x + m_dimensions.margin.left + m_dimensions.border.left + m_dimensions.padding.left;
	m_dimensions.content.y = container.content.height + container.content.y + m_dimensions.margin.top +
	                         m_dimensions.border.top + m_dimensions.padding.top;
}

void Block::calculate_height(Box container) { }

std::optional<::Node *> Block::hit_test(const Point &p)
{
	if (!m_inline_format_context)
		return Node::hit_test(p);

	for (auto const &line : lines)
	{
		int x = line.x;
		int y = line.y;
		int height = line.height;
		for (auto const &fragment : line.fragments)
		{
			if (Rect(x + fragment.offset, y, fragment.len, height).contains(p))
				return { fragment.styled_node->node() };
		}
	}
	return {};
}

std::string Block::to_string() const
{
	std::stringstream ss;
	ss << "Block";
	if (is_anonymous())
		ss << "<anon>";
	else
		ss << "<" << tag_name() << ">";

	ss << " @ ";
	ss << "(" << m_dimensions.content.x << ", " << m_dimensions.content.y << ") ";
	ss << "[" << m_dimensions.content.width << "x" << m_dimensions.content.height << "]";

	if (m_inline_format_context)
		ss << " (inline children)";
	else
		ss << " (block children)";

	return ss.str();
}
}
