#include "block.h"

namespace layout
{
Block::Block() :
    Node(nullptr)
{
	m_anonymous = true;
}

Block::Block(std::shared_ptr<css::StyledNode> node) :
    Node(node)
{
	m_anonymous = false;
}

void Block::layout(Box container) { }
void Block::calculate_width(Box container) { }
void Block::calculate_position(Box container) { }
void Block::calculate_height(Box container) { }

std::string Block::to_string() const
{
	std::string res = "Block";
	if (is_anonymous())
		res += "<anon>";
	else
		res += "<" + tag_name() + ">";
	
	if (m_inline_format_context)
		res += " (inline formatting)";
	else
		res += " (block formatting)";

	return res;
}
}
