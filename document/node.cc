#include "node.h"

#include "element.h"

bool Node::is_link() const
{
	for (auto *tree_node = parent(); tree_node; tree_node = tree_node->parent())
	{
		auto *node = static_cast<Node *>(tree_node);
		if (node->type() == NodeType::Element)
		{
			auto *element = static_cast<Element*>(node);
			if (element->tag() == "a")
				return true;
		}
	}

	return false;
}

NodeType Node::type() const
{
	return NodeType::Node;
}

std::string Node::to_string() const
{
	return "HTML Node";
}

std::string Node::element_name() const
{
	if (type() == NodeType::Element)
	{
		auto *element = static_cast<Element const*>(this);
		return element->tag();
	}

	return "<non-element>";
}
