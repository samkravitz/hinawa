#include "stylesheet.h"

#include <fstream>
#include <sstream>

#include "parser.h"

namespace css
{
Stylesheet read_default_stylesheet()
{
	std::ifstream file("../data/default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	return Parser::parse(buffer.str());
}

std::vector<Declaration> Stylesheet::rules_for_tag(std::string const tag) const
{
	std::vector<Declaration> declarations;

	for (auto rule : rules)
	{
		for (auto selector : rule.selectors)
		{
			if (selector.tag_name == tag)
			{
				for (auto declaration : rule.declarations)
				{
					declarations.push_back(declaration);
				}
			}
		}
	}

	return declarations;
}

std::vector<Declaration> Stylesheet::rules_for_class(std::string const class_name) const
{
	std::vector<Declaration> declarations;

	for (auto rule : rules)
	{
		for (auto selector : rule.selectors)
		{
			if (selector.class_name == class_name)
			{
				for (auto declaration : rule.declarations)
				{
					declarations.push_back(declaration);
				}
			}
		}
	}

	return declarations;
}

std::vector<Declaration> Stylesheet::universal_rules() const
{
	std::vector<Declaration> declarations;

	for (auto rule : rules)
	{
		for (auto selector : rule.selectors)
		{
			if (selector.is_universal)
			{
				for (auto declaration : rule.declarations)
				{
					declarations.push_back(declaration);
				}
			}
		}
	}

	return declarations;
}
}
