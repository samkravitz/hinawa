#pragma once

#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>

#include "component_value.h"
#include "selector.h"
#include "value.h"

namespace css
{
class StyledNode;

// ex
// margin: auto
struct Declaration
{
	std::string name;
	std::vector<ComponentValue> value;
	bool important{false};

	Value* style_value() const;

	std::string value_text() const
	{
		std::string s = "";
		for (const auto &cv : value)
			s += cv.token.value();
		return s;
	}

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

	std::string to_string() const
	{
		std::string res = "";
		for (const auto &cv : value)
			res += cv.token.value();

		return res;
	}
};

struct QualifiedRule
{
	std::vector<ComponentValue> prelude;
	ParserBlock block;

	std::string to_string() const
	{
		std::string res = "QualifiedRule:\n";
		res += "Prelude:\n";
		for (const auto &cv : prelude)
			res += cv.token.value();
		res += "\n";
		res += "Block:\n";
		res += block.to_string();
		return res;
	}
};

struct Rule
{
	Selector selector() const;
	std::vector<Declaration> declarations() const;

	QualifiedRule qualified_rule;

	std::string to_string() const { return qualified_rule.to_string(); }
};

struct Stylesheet
{
	Stylesheet() = default;

	std::vector<Rule> rules;

	void style(StyledNode*) const;
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
