#pragma once

#include "node.h"

namespace layout
{
class Inline : public Node
{
public:
	Inline(std::shared_ptr<css::StyledNode>);
	Inline() = delete;

	void layout(Box);
	bool is_inline() const { return true; }
	std::string to_string() const;

private:
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);
};
}
