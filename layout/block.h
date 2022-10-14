#pragma once

#include "node.h"

namespace layout
{
class Block : public Node
{
public:
	Block(std::shared_ptr<css::StyledNode>);
	Block();

	void layout(Box);
	std::string to_string() const;
	inline bool is_anonymous() const { return m_anonymous; };

private:
	bool m_anonymous = true;
	void calculate_width(Box);
	void calculate_position(Box);
	void calculate_height(Box);
};
}
