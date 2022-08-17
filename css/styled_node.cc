#include "styled_node.h"

#include "../document/element.h"

#include <iostream>

namespace css
{
StyledNode::StyledNode(std::shared_ptr<Node> node) : m_node(node) { }

StyledNode::StyledNode(std::shared_ptr<Node> node, std::shared_ptr<Stylesheet> stylesheet)
{
	m_node = node;
	if (node->type() == NodeType::Element)
	{
		auto element = std::dynamic_pointer_cast<Element>(node);
		for (auto decl : stylesheet->rules_for_tag(element->tag()))
		{
			m_values[decl.name] = decl.value;
		}

		if (element->has_attribute("class"))
		{
			for (auto decl : stylesheet->rules_for_class(element->get_attribute("class")))
			{
				m_values[decl.name] = decl.value;
			}
		}
		
	}

	auto f = [this, stylesheet](std::shared_ptr<Node> node)
	{
		children.push_back(std::make_shared<StyledNode>(StyledNode(node, stylesheet)));
	};

	node->for_each_child(f);
}
}
