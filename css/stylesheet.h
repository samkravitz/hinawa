#pragma once

#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>

#include "component_value.h"
#include "value.h"
#include "selector.h"

namespace css
{
class StyledNode;

// ex
// margin: auto
struct Declaration
{
	std::string name;
	std::vector<ComponentValue> value;
	bool important{ false };

	Value *style_value() const { return nullptr; }

	std::string to_string() const
	{
		std::string s = "";
		for (const auto &cv : value)
			s += fmt::format("{} ", cv.token.value());
		return fmt::format("{}: '{}'", name, s);
	}
};

struct ParserBlock
{
	Token associated_token;
	std::vector<ComponentValue> value;
};

struct QualifiedRule
{
	std::vector<ComponentValue> prelude;
	ParserBlock block;
};

struct Rule
{
	std::vector<Selector> selectors;
	std::vector<Declaration> declarations;

	QualifiedRule qualified_rule;

	std::string to_string() const
	{
		std::string res = "Selectors:\n";
		for (const auto &selector : selectors)
			res += fmt::format("\t{}\n", selector.to_string());

		res += "Declarations:\n";

		for (const auto &declaration : declarations)
			res += fmt::format("\t{}\n", declaration.to_string());

		return res;
	}
};

struct Stylesheet
{
	Stylesheet() = default;

	std::vector<Rule> rules;

	void style(StyledNode *) const;
	void print() const
	{
		fmt::print("Stylesheet:\n");
		for (const auto &rule : rules)
		{
			fmt::print("Rule:\n");
			fmt::print("{}\n", rule.to_string());
		}
	}
};
}
