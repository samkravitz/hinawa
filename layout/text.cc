#include "text.h"

namespace layout
{
Text::Text(css::StyledNode *node) :
    Node(node)
{ }

void Text::layout(Box container) { }
void Text::calculate_width(Box container) { }
void Text::calculate_position(Box container) { }
void Text::calculate_height(Box container) { }
std::string Text::to_string() const { return "Text"; }
}
