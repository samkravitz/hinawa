#pragma once

#include "node.h"

namespace layout
{
class Text : public Node
{
public:
	Text(css::StyledNode*);
	Text() = delete;

	void layout(Box);
	bool is_inline() const { return true; }
	bool is_text() const { return true; }
	std::string to_string() const;

private:
	void split_into_lines(Box container);
};
}
