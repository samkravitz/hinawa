#include "styled_node.h"

#include "../document/element.h"

#include <iostream>

namespace css
{
StyledNode::StyledNode(std::shared_ptr<Node> node) : node(node) { }

StyledNode::StyledNode(std::shared_ptr<Node> n, std::shared_ptr<Stylesheet> stylesheet)
{
	node = n;
	if (n->type() == NodeType::Element)
	{
		auto element = std::dynamic_pointer_cast<Element>(n);
		for (auto decl : stylesheet->rules_for(element->tag()))
		{
			values[decl.name] = decl.value;
		}
	}

	auto f = [this, stylesheet](std::shared_ptr<Node> n)
	{
		children.push_back(std::make_shared<StyledNode>(StyledNode(n, stylesheet)));
	};

	n->for_each_child(f);
}
}
