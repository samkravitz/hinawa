#include "stylesheet.h"

#include <cassert>

#include "document/element.h"
#include "parser.h"
#include "styled_node.h"

namespace css
{
Selector Rule::selector() const
{
	auto selector = Parser::parse_selector_list(qualified_rule.prelude);
	assert(selector);
	selector->print();
	return *selector;
}

std::vector<Declaration> Rule::declarations() const
{
	return Parser::parse_declaration_list(qualified_rule.block.value);
}

void Stylesheet::style(StyledNode *styled_node) const
{
	assert(styled_node);
	for (const auto &rule : rules)
	{
		auto selector = rule.selector();
		if (selector.matches(*styled_node))
		{
			for (const auto &declaration : rule.declarations())
			{
				std::cout << declaration.name << " hiii "
				          << "\n";
				styled_node->assign(declaration.name, declaration.style_value());
			}
		}
	}
}

Value *Declaration::style_value() const
{
	return Parser::parse_style_value(name, value);
}
}
