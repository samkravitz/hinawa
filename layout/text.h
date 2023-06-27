#pragma once

#include "node.h"

#include "SkFont.h"

namespace layout
{
class Text : public Node
{
public:
	Text(css::StyledNode *);
	Text() = delete;

	void layout(Box);
	bool is_inline() const { return true; }
	bool is_text() const { return true; }

	SkFont font() const { return m_font; }

	std::string to_string() const;

private:
	void split_into_lines(Box container);

	SkFont m_font{};
};
}
