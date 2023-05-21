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

		bool matches(const StyledNode &) const;
		void print() const;
	};

	enum class Combinator
	{
		None,
		Child,              // >
		Descendant,         // <whitespace>
		AdjacentSibling,    // +
		GeneralSibling,     // ~
	};

	struct CompoundSelector
	{
		std::vector<SimpleSelector> simple_selectors;
		Combinator combinator{Combinator::None};

		bool matches(const StyledNode &) const;
		void print() const;
	};

	struct ComplexSelector
	{
		std::vector<CompoundSelector> compound_selectors;

		bool matches(const StyledNode &) const;
		void print() const;
	};

	std::vector<ComplexSelector> complex_selectors;

	bool matches(const StyledNode &) const;

	bool is_selector_list() const { return complex_selectors.size() > 1; }
	bool is_complex() const;
	bool is_compound() const;
	bool is_simple() const;

	void print() const;
};
}