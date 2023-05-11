#include "selector.h"

#include <cassert>

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

}
