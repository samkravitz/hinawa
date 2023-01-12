#pragma once

#include "document.h"
#include "util/tree_node.h"

enum class NodeType
{
	Node,
	Element,
	Text,
};

class Node : public util::TreeNode<Node>
{
public:
	Node() = default;
	Node(Document &);
	bool is_link() const;
	virtual NodeType type() const;
	std::string element_name() const;
	std::string to_string() const;
	Document &document();

private:
	Document m_document;
};
