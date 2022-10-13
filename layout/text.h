#pragma once

#include "node.h"

namespace layout
{
class Text : public Node
{
public:
	Text(std::shared_ptr<css::StyledNode>);
	Text() = delete;

	void layout(Box);
	std::string to_string() const;

private:
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);
};
}
