#include "text.h"

#include <cassert>

#include "block.h"
#include "browser/browser.h"
#include "document/text.h"

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

	auto *font_size = dynamic_cast<css::Length *>(node()->lookup("font-size"));
	auto *text_element = dynamic_cast<::Text *>(node()->node());
	auto str = text_element->trim();
	auto px = font_size->to_px();
	const int max_width = container.content.width;
	int current_x = container.content.x;
	int current_y = container.content.y;

	if (lines.empty())
		lines.push_back(Line(current_x, current_y));
	
	current_x += lines.back().width;
	
	int available_width = max_width - lines.back().width;

	auto space = sf::Text(" ", font, px);
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(px);

	std::istringstream ss(str);
	std::string word;
	LineFragment frag;
	frag.styled_node = m_node;

	while (std::getline(ss, word, ' '))
	{
		text.setString(word);
		float len = text.getLocalBounds().width;
		float height = text.getLocalBounds().height;

		// fragment would overflow the max allowed width, so it must be put on a new line
		if (current_x + len > available_width)
		{
			available_width = max_width;
			frag.offset = current_x - container.content.x - frag.len;
			current_x = container.content.x;
			current_y += lines.back().height;

			lines.back().fragments.push_back(frag);
			lines.push_back(Line(current_x, current_y));
			frag = LineFragment();
			frag.styled_node = m_node;
		}

		frag.str += word + " ";
		frag.len += len + space.getLocalBounds().width;
		current_x += len;
		current_x += space.getLocalBounds().width;

		lines.back().width += len;
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
	}
}

std::string Text::to_string() const
{
	std::stringstream ss;
	ss << "Text";

	ss << " @ ";
	ss << "(" << m_dimensions.content.x << ", " << m_dimensions.content.y << ") ";
	ss << "[" << m_dimensions.content.width << "x" << m_dimensions.content.height << "]";

	return ss.str();
}
}
