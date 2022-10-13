#include "block.h"

namespace layout
{
Block::Block() :
    Node(nullptr)
{ }

Block::Block(std::shared_ptr<css::StyledNode> node) :
    Node(node)
{ }

void Block::layout(Box container) { }
void Block::calculate_width(Box container) { }
void Block::calculate_position(Box container) { }
void Block::calculate_height(Box container) { }
std::string Block::to_string() const { return "Block"; }
}
