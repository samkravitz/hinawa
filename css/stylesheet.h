#pragma once

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

	std::string to_string() { return ""; }
};

// ex
// margin: auto
struct Declaration
{
	std::string name;
	Value *value;

	std::string to_string() { return name + " : " + value->to_string(); }
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

	void style(StyledNode *) const;

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
