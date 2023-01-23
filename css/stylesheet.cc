#include "stylesheet.h"

#include <fstream>
#include <sstream>

#include "document/element.h"
#include "parser.h"
#include "styled_node.h"

namespace css
{
Stylesheet read_default_stylesheet()
{
	std::ifstream file("../data/default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	return Parser::parse(buffer.str());
}

void Stylesheet::style(StyledNode *styled_node) const
{
	for (const auto &rule : rules)
	{
		for (const auto &selector : rule.selectors)
		{
			if (selector.matches(styled_node))
			{
				for (const auto &declaration : rule.declarations)
					styled_node->assign(declaration.name, declaration.value);
			}
		}
	}
}

bool Selector::matches(StyledNode *styled_node) const
{
	switch (type)
	{
		case Type::Simple: return simple_selector.matches(styled_node);

		case Type::Compound:
		{
			for (const auto &simple : compound_selector.simple_selectors)
			{
				if (!simple.matches(styled_node))
					return false;
			}
			return true;
		}
	}
}

bool SimpleSelector::matches(StyledNode *styled_node) const
{
	auto *dom_node = styled_node->node();
	if (!dom_node)
		return false;
	switch (type)
	{
		case Type::Universal: return true;
		case Type::Type: return dom_node->element_name() == value;
		case Type::Class:
		{
			if (dom_node->type() != NodeType::Element)
				return false;

			auto *element = static_cast<Element *>(dom_node);
			if (!element->has_attribute("class"))
				return false;

			return element->get_attribute("class") == value;
		}

		case Type::Id:
		{
			if (dom_node->type() != NodeType::Element)
				return false;

			auto *element = static_cast<Element *>(dom_node);
			if (!element->has_attribute("id"))
				return false;

			return element->get_attribute("id") == value;
		}
	}
	return false;
}
}
