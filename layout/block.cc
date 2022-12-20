#include "block.h"

#include <sstream>

#include "browser/browser.h"
#include "document/text.h"

extern sf::Font font;

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
	m_dimensions = Box();
	lines.clear();

	calculate_width(container);
	calculate_position(container);

	if (m_inline_format_context)
	{
		auto offset = Point{ m_dimensions.content.x, m_dimensions.content.y };
		preorder([&](auto *child)
		{
			if (child->is_text())
			{
				offset = split_into_lines(m_dimensions, offset, child);

				// remove trailing " " from fragment
				if (!lines.empty())
				{
					auto &frag = lines.back().fragments.back();
					frag.str.pop_back();

					// len(" ") is usually about 4 px, but that's just an approximation
					frag.len -= 4;
				}
			}
		});

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
		m_node = parent()->node();
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

void Block::calculate_position(Box container)
{
	if (m_anonymous)
		m_node = parent()->node();
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

void Block::calculate_height(Box container) { }

Point Block::split_into_lines(const Box &container_start, const Point &offset, Node *text_node)
{
	auto *font_size = dynamic_cast<css::Length *>(text_node->node()->lookup("font-size"));

	auto *text_element = dynamic_cast<Text *>(text_node->node()->node());
	auto str = text_element->trim();
	auto px = font_size->to_px();
	const int max_width = container_start.content.width;
	int current_x = offset.x;
	int current_y = offset.y;

	if (lines.empty())
		lines.push_back(Line(current_x, current_y));

	auto space = sf::Text(" ", font, px);
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(px);

	std::istringstream ss(str);
	std::string word;
	LineFragment frag;
	frag.styled_node = text_node->node();

	while (std::getline(ss, word, ' '))
	{
		text.setString(word);
		float len = text.getLocalBounds().width;
		float height = text.getLocalBounds().height;

		// fragment would overflow the max allowed width, so it must be put on a new line
		if (current_x + len > max_width)
		{
			frag.offset = current_x - container_start.content.x - frag.len;
			current_x = container_start.content.x;
			current_y += lines.back().height;

			lines.back().fragments.push_back(frag);
			lines.push_back(Line(current_x, current_y));
			frag = LineFragment();
			frag.styled_node = text_node->node();
		}

		frag.str += word + " ";
		frag.len += len + space.getLocalBounds().width;
		current_x += len;
		current_x += space.getLocalBounds().width;

		lines.back().width += len;
		lines.back().height = std::max((float) lines.back().height, height);
	}

	if (!frag.str.empty())
	{
		frag.offset = current_x - container_start.content.x - frag.len;
		lines.back().fragments.push_back(frag);
	}

	return Point{ current_x, current_y };
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
