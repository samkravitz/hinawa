#include <gtest/gtest.h>

#include "css/selector.h"

TEST(SelectorTest, BasicAssertions)
{
	css::Selector s = {};
	EXPECT_FALSE(s.is_selector_list());
}
