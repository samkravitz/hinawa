#include "text.h"

#include <cassert>

#include "block.h"
#include "document/text.h"
#include "value.h"

#include "SkFont.h"
#include "SkFontMgr.h"
#include "SkTextBlob.h"

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
			block_ancestor = static_cast<Block *>(p);
			break;
		}
	}
	assert(block_ancestor);
	auto &lines = block_ancestor->lines;

	auto *font_size = style()->property("font-size");
	float px = font_size->font_size();

	auto *text_element = dynamic_cast<::Text *>(style()->node());
	auto str = text_element->trim();

	const int max_x = container.content.width + container.content.x;

	if (lines.empty())
		lines.push_back(Line(container.content.x, container.content.y));

	int current_x = lines.back().x + lines.back().width;
	int current_y = lines.back().y;

	const char *fontFamily = nullptr;
	SkFontStyle fontStyle;
	auto fontManager = SkFontMgr::RefDefault();
	auto typeface = fontManager->legacyMakeTypeface(fontFamily, fontStyle);
	m_font = SkFont(typeface, px);

	auto space = SkTextBlob::MakeFromString(" ", m_font);
	const auto space_width = space->bounds().width();

	std::istringstream ss(str);
	std::string word;
	LineFragment frag = {};
	frag.styled_node = m_style;
	frag.text_node = this;

	auto *white_space = dynamic_cast<css::Keyword *>(style()->property("white-space"));
	// preserve whitespace
	if (white_space && white_space->value == "pre")
	{
		str = text_element->text();
		ss = std::istringstream(str);

		while (std::getline(ss, word, '\n'))
		{
			auto blob = SkTextBlob::MakeFromString(word.c_str(), m_font);
			float len = blob->bounds().width();
			float height = px;

			frag.str = word;
			frag.len = len;
			frag.styled_node = m_style;
			frag.text_node = this;

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
		std::string s(frag.str + word);
		auto blob = SkTextBlob::MakeFromString(word.c_str(), m_font);

		float len = blob->bounds().width();
		float height = px;

		// fragment would overflow the max allowed width, so it must be put on a new line
		if (current_x + len > max_x)
		{
			frag.offset = std::max((float) (current_x - container.content.x - frag.len), 0.0f);
			current_x = container.content.x;
			current_y += lines.back().height;

			lines.back().fragments.push_back(frag);
			lines.push_back(Line(current_x, current_y));
			frag = {};
			frag.styled_node = m_style;
			frag.text_node = this;
		}

		frag.str += word + " ";
		frag.len += len + space_width;
		current_x += len;
		current_x += space_width;

		lines.back().width += len + space_width;
		lines.back().height = std::max((float) lines.back().height, height);

		m_dimensions.content.width = std::max((float) m_dimensions.content.width, (float) frag.len);
		m_dimensions.content.height = std::max((float) m_dimensions.content.height, height);
	}

	if (!frag.str.empty())
	{
		frag.offset = std::max((float) (current_x - container.content.x - frag.len), 0.0f);
		lines.back().fragments.push_back(frag);
	}

	auto *text_align = style()->property("text-align");
	if (auto *keyword = dynamic_cast<css::Keyword *>(text_align); keyword && keyword->value == "center")
	{
		for (auto &line : lines)
		{
			int center_x_offset = (max_x - container.content.x - line.width) / 2;
			line.x += center_x_offset;
		}
	}
}

std::string Text::to_string() const
{
	return "Text";
}
}
