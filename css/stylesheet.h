#pragma once

#include <memory>
#include <string>
#include <vector>

namespace css
{
// ex
// div
// p, h1
struct Selector
{
	std::string tag_name;
	std::string id;
	std::string class_name;

	std::string to_string()
	{
		return tag_name;
	}
};


// ex
// margin: auto
struct Declaration
{
	std::string name;
	std::string value;

	std::string to_string()
	{
		return name + " : " + value;
	}
};

struct Rule
{
	std::vector<Selector> selectors;
	std::vector<Declaration> declarations;

	std::string to_string()
	{
		std::string res = "";
		for (auto selector : selectors)
		{
			res += selector.to_string() + " ";
		}

		res += "{\n";

		for (auto declaration : declarations)
		{
			res += "\t" + declaration.to_string() + "\n";
		}

		res += "}\n";

		return res;
	}
};

struct Stylesheet
{
	Stylesheet() = default;

	std::vector<Rule> rules;

	std::vector<Declaration> rules_for(std::string const tag)
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

	std::string to_string()
	{
		std::string res = "Stylesheet\n";
		for (auto rule : rules)
		{
			res += rule.to_string() + "\n";
		}

		return res;
	}
};

std::shared_ptr<Stylesheet> read_default_stylesheet();
}
