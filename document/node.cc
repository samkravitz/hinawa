#include "node.h"

#include <iostream>

NodeType Node::type() const
{
	return NodeType::Node;
}

void Node::print(int depth) const
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";

	std::cout << "HTML Node" << "\n";

	for (auto child : children)
		child->print(depth + 1);
}
