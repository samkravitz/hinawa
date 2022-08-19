#include "node.h"

NodeType Node::type() const
{
	return NodeType::Node;
}

std::string Node::to_string() const
{
	return "HTML Node";
}
