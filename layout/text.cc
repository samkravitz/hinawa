#include "text.h"

#include <cassert>

#include "block.h"
#include "browser/browser.h"
#include "document/text.h"
#include "value.h"

extern sf::Font font;

namespace layout
{
Text::Text(css::StyledNode *node) :
    Node(node)
{ }

void Text::layout(Box container)
{
	m_dimensions = Box();
	split_into_lines(container);
}

void Text::split_into_lines(Box container)
{
	// nearest ancestor that is block layout
	Block *block_ancestor = nullptr;
	for (auto *p = parent(); p; p = p->parent())
	{
		if (p->is_block())
		{
			block_ancestor = static_cast<Block*>(p);
			break;
		}
	}
	assert(block_ancestor);
	auto &lines = block_ancestor->lines;

	auto *font_size = style()->property("font-size");
	float px = font_size->font_size();

	auto *text_element = dynamic_cast<::Text *>(style()->node());
	auto str = text_element->trim();

	const int max_width = container.content.width;

	if (lines.empty())
		lines.push_back(Line(container.content.x, container.content.y));

	int current_x = lines.back().x + lines.back().width;
	int current_y = lines.back().y;

	auto space = sf::Text(" ", font, px);
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(px);

	std::istringstream ss(str);
	std::string word;
	LineFragment frag = {};
	frag.styled_node = m_style;

	auto *white_space = dynamic_cast<css::Keyword *>(style()->property("white-space"));
	// preserve whitespace
	if (white_space && white_space->value == "pre")
	{
		str = text_element->text();
		ss = std::istringstream(str);

		while (std::getline(ss, word, '\n'))
		{
			text.setString(word);
			float len = text.getLocalBounds().width;
			float height = px;

			frag.str = word;
			frag.len = len;
			frag.styled_node = m_style;
			lines.back().fragments.push_back(frag);
			lines.back().height = height;
			lines.back().width = len;

			current_y += height;
			lines.push_back(Line(current_x, current_y));
			m_dimensions.content.width = std::max((float) m_dimensions.content.width, (float) frag.len);
			m_dimensions.content.height += height;
		}
		return;
	}

	while (std::getline(ss, word, ' '))
	{
		text.setString(word);
		float len = text.getLocalBounds().width;
		float height = px;

		// fragment would overflow the max allowed width, so it must be put on a new line
		if (current_x + len > max_width)
		{
			frag.offset = current_x - container.content.x - frag.len;
			current_x = container.content.x;
			current_y += lines.back().height;

			lines.back().fragments.push_back(frag);
			lines.push_back(Line(current_x, current_y));
			frag = {};
			frag.styled_node = m_style;
		}

		frag.str += word + " ";
		frag.len += len + space.getLocalBounds().width;
		current_x += len;
		current_x += space.getLocalBounds().width;

		lines.back().width += len + space.getLocalBounds().width;
		lines.back().height = std::max((float) lines.back().height, height);

		m_dimensions.content.width = std::max((float) m_dimensions.content.width, (float) frag.len);
		m_dimensions.content.height = std::max((float) m_dimensions.content.height, height);
	}

	if (!frag.str.empty())
	{
		frag.offset = current_x - container.content.x - frag.len;
		lines.back().fragments.push_back(frag);
	}

	// remove trailing " " from fragment
	if (!lines.empty())
	{
		auto &frag = lines.back().fragments.back();
		frag.str.pop_back();

		// len(" ") is usually about 4 px, but that's just an approximation
		frag.len -= 4;
		lines.back().width -= 4;
	}

	auto *text_align = style()->property("text-align");
	if (auto *keyword = dynamic_cast<css::Keyword*>(text_align); keyword && keyword->value == "center")
	{
		for (auto &line : lines)
		{
			int center_x_offset = (max_width - line.width) / 2;
			line.x += center_x_offset;
		}
	}
}

std::string Text::to_string() const
{
	return "Text";
}
}
