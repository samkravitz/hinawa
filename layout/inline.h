#pragma once

#include "node.h"

namespace layout
{
class Inline : public Node
{
public:
	Inline(css::StyledNode *);
	Inline() = delete;

	void layout(Box);
	bool is_inline() const { return true; }
	std::string to_string() const;
};
}
