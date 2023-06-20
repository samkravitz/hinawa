#include "inline.h"

namespace layout
{
Inline::Inline(css::StyledNode* node) :
    Node(node)
{ }

void Inline::layout(Box container)
{
	for_each_child([&](auto* child) { child->layout(container); });
}

std::string Inline::to_string() const
{
	std::stringstream ss;
	ss << "Inline";
	ss << "<" << tag_name() << ">";

	return ss.str();
}
}
