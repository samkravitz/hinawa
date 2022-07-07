#include "node.h"

void Node::add_child(std::shared_ptr<Node> node)
{
	children.push_back(node);
}
