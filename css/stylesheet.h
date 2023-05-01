#pragma once

#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>

#include "value.h"

namespace css
{
class StyledNode;

// https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Selectors
struct SimpleSelector
{
	enum class Type
	{
		Universal,
		Type,
		Class,
		Id,
		Attribute,
		PseudoClass,
		PseudoElement,
	} type;

	std::string value;

	bool matches(StyledNode *styled_node) const;
};

struct CompoundSelector
{
	std::vector<SimpleSelector> simple_selectors;
};

struct SelectorList
{
	std::vector<CompoundSelector> compound_selectors;
};

struct ComplexSelector
{ };

// ex
// div
// p, h1
struct Selector
{
	enum class Type
	{
		Simple,
		Compound,
		Complex,
		Relative,
	} type;

	SimpleSelector simple_selector;
	CompoundSelector compound_selector;
	ComplexSelector complex_selector;

	bool matches(StyledNode *) const;

	std::string to_string() const { return ""; }
};

// ex
// margin: auto
struct Declaration
{
	std::string name;
	std::string value;

	Value *style_value() const { return nullptr; }

	std::string to_string() const { return fmt::format("{}: '{}'", name, value); }
};

struct Rule
{
	std::vector<Selector> selectors;
	std::vector<Declaration> declarations;

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
