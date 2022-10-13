#include "inline.h"

namespace layout
{
Inline::Inline(std::shared_ptr<css::StyledNode> node) :
    Node(node)
{ }

void Inline::layout(Box container) { }
void Inline::calculate_width(Box container) { }
void Inline::calculate_position(Box container) { }
void Inline::calculate_height(Box container) { }
std::string Inline::to_string() const { return "Inline"; }
}
