#pragma once

#include <memory>
#include <string>
#include <vector>

#include "value.h"

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

	bool is_universal = false;

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
	Value *value;

	std::string to_string()
	{
		return name + " : " + value->to_string();
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

	std::vector<Declaration> universal_rules() const;
	std::vector<Declaration> rules_for_tag(std::string const) const;
	std::vector<Declaration> rules_for_class(std::string const) const;

	std::string to_string() const
	{
		std::string res = "Stylesheet\n";
		for (auto rule : rules)
		{
			res += rule.to_string() + "\n";
		}

		return res;
	}
};

Stylesheet read_default_stylesheet();
}
