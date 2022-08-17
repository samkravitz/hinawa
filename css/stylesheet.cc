#include "stylesheet.h"

#include <fstream>
#include <sstream>

#include "parser.h"

namespace css
{
std::shared_ptr<Stylesheet> read_default_stylesheet()
{
	std::ifstream file("css/default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = Parser(buffer.str());
	return parser.parse();
}

std::vector<Declaration> Stylesheet::rules_for_tag(std::string const tag)
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

std::vector<Declaration> Stylesheet::rules_for_class(std::string const class_name)
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
}
