#include "stylesheet.h"

#include "document/element.h"
#include "parser.h"
#include "styled_node.h"

namespace css
{
void Stylesheet::style(StyledNode *styled_node) const
{
	for (const auto &rule : rules)
	{
		for (const auto &selector : rule.selectors)
		{
			if (selector.matches(styled_node))
			{
				for (const auto &declaration : rule.declarations)
					styled_node->assign(declaration.name, declaration.style_value());
			}
		}
	}
}
}
