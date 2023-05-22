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

	auto selector = Parser::parse_selector(css);

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

TEST(SelectorParseTests, ParseCompound)
{
	std::string css = R"(div.my_class {})";

	auto selector = Parser::parse_selector(css);

	EXPECT_FALSE(selector->is_selector_list());
	EXPECT_TRUE(selector->is_compound());
	EXPECT_EQ(selector->complex_selectors.size(), 1);

	auto complex = selector->complex_selectors[0];
	EXPECT_EQ(complex.compound_selectors.size(), 1);

	auto compound = complex.compound_selectors[0];
	EXPECT_EQ(compound.simple_selectors.size(), 2);
	EXPECT_EQ(compound.combinator, Selector::Combinator::None);

	auto div = compound.simple_selectors[0];
	auto my_class = compound.simple_selectors[1];

	EXPECT_EQ(div.type, Selector::SimpleSelector::Type::Type);
	EXPECT_EQ(div.value, "div");
	EXPECT_EQ(my_class.type, Selector::SimpleSelector::Type::Class);
	EXPECT_EQ(my_class.value, "my_class");
}
}
