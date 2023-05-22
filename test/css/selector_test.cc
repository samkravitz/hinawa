#include <gtest/gtest.h>

#include <string>

#include "css/parser.h"
#include "css/selector.h"

namespace css
{
TEST(SelectorTest, BasicAssertions)
{
	Selector s = {};
	EXPECT_FALSE(s.is_selector_list());
}

TEST(SelectorParseTests, ParseSimpleList)
{
	std::string css = R"(body,
		div,
		h1,
		h2,
		h3,
		h4,
		h5,
		h6,
		p,
		ul {})";

	auto selector = Parser::parse_selector_list(css);

	EXPECT_TRUE(selector->is_selector_list());
	EXPECT_EQ(selector->complex_selectors.size(), 10);

	for (const auto &complex : selector->complex_selectors)
	{
		EXPECT_EQ(complex.compound_selectors.size(), 1);
		for (const auto &compound : complex.compound_selectors)
		{
			EXPECT_EQ(compound.simple_selectors.size(), 1);
			auto simple = compound.simple_selectors[0];
			EXPECT_EQ(simple.type, Selector::SimpleSelector::Type::Type);
		}
	}
}
}
