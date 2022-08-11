#pragma once

#include "../util/tree_node.h"

enum class NodeType
{
	Node,
	Element,
	Text,
};

class Node : public util::TreeNode<Node>
{
public:
	virtual NodeType type() const;
	virtual void print(int depth = 0) const;
};
