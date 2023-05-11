/**
* This selector class represents the <selector-list> production from
* https://www.w3.org/TR/selectors-4/#grammar
*
*/

#pragma once

#include <string>
#include <vector>

namespace css
{
class StyledNode;

using SelectorList = std::vector<class Selector>;

class Selector
{
public:
	struct SimpleSelector
	{
		enum class Type
		{
			Universal,        // *
			Type,             // <tagname>
			Class,            // .
			Id,               // #
			Attribute,        // []
			PseudoClass,      // :
			PseudoElement,    // ::
		} type;

		std::string value;

		bool matches(StyledNode *styled_node) const { return false; }
	};

	struct CompoundSelector
	{
		std::vector<SimpleSelector> simple_selectors;
	};

	enum class Combinator
	{
		None,
		Child,              // >
		Descendant,         // <whitespace>
		AdjacentSibling,    // +
		GeneralSibling,     // ~
	};

	struct ComplexSelector
	{
		CompoundSelector compound_selector;
		Combinator combinator{Combinator::None};
	};

	std::vector<ComplexSelector> complex_selectors;

	bool is_selector_list() const { return complex_selectors.size() > 1; }
	bool is_complex() const;
	bool is_compound() const;
	bool is_simple() const;

	bool matches(StyledNode *) const { return false; }
	std::string to_string() const { return ""; }
};
}