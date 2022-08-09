#include "node.h"

#include <iostream>

void Node::add_child(std::shared_ptr<Node> node)
{
	children.push_back(node);
}

void Node::preorder(std::function<void(std::shared_ptr<Node>)> f)
{
	for (auto child : children)
	{
		child->preorder(f);
		f(child);
	}
}

void Node::for_each_child(std::function<void(std::shared_ptr<Node>)> f)
{
	for (auto child : children)
		f(child);
}

bool Node::has_children()
{
	return !children.empty();
}

std::shared_ptr<Node> Node::last_child()
{
	if (!has_children())
		return nullptr;
	
	return children.back();
}

NodeType Node::type()
{
	return NodeType::Node;
}

void Node::print(int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "\t";

	std::cout << "HTML Node" << "\n";

	for (auto child : children)
		child->print(depth + 1);
}
