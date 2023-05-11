#include "selector.h"

#include <cassert>
#include <fmt/format.h>

namespace css
{
bool Selector::is_complex() const
{
	assert(!complex_selectors.empty());

	if (is_selector_list())
		return false;

	auto selector = complex_selectors[0];
	return selector.combinator != Combinator::None;
}

bool Selector::is_compound() const
{
	if (is_selector_list())
		return false;

	if (is_complex())
		return false;

	auto compound_selector = complex_selectors[0].compound_selector;
	assert(!compound_selector.simple_selectors.empty());
	return compound_selector.simple_selectors.size() > 1;
}

bool Selector::is_simple() const
{
	if (is_selector_list())
		return false;

	if (is_complex())
		return false;

	if (is_compound())
		return false;

	return true;
}

void Selector::SimpleSelector::print() const
{
	std::string t;
	switch (type)
	{
		case Type::Universal:
			t = "universal";
			break;
		case Type::Type:
			t = "type";
			break;
		case Type::Class:
			t = "class";
			break;
		case Type::Id:
			t = "id";
			break;
		case Type::Attribute:
			t = "attribute";
			break;
		case Type::PseudoClass:
			t = "pseudo class";
			break;
		case Type::PseudoElement:
			t = "pseudo element";
			break;
	}

	fmt::print("Type: simple\n");
	fmt::print("type: {} value: {}\n", t, value);
}

void Selector::CompoundSelector::print() const
{
	assert(!simple_selectors.empty());
	if (simple_selectors.size() == 1)
	{
		auto simple_selector = simple_selectors[0];
		simple_selector.print();
		return;
	}

	fmt::print("Type: compound\n");
	for (const auto &simple_selector : simple_selectors)
		simple_selector.print();
}

void Selector::ComplexSelector::print() const
{
	if (combinator == Combinator::None)
	{
		compound_selector.print();
		return;
	}

	std::string com = "";
	switch (combinator)
	{
		case Combinator::Child:
			com = ">";
			break;
		case Combinator::Descendant:
			com = "\" \"";
			break;
		case Combinator::AdjacentSibling:
			com = "+";
			break;
		case Combinator::GeneralSibling:
			com = "~";
			break;
		default:
			assert(!"Unknown combinator");
	}

	fmt::print("Type: complex\n Combinator: {}\n", com);
	compound_selector.print();
}

void Selector::print() const
{
	if (is_selector_list())
		fmt::print("Type: list\n");

	for (const auto &complex_selector : complex_selectors)
		complex_selector.print();

	fmt::print("\n");
}

}
