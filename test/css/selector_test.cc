#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "css/parser.h"
#include "css/selector.h"
#include "css/styled_node.h"
#include "document/element.h"
#include "document/element_factory.h"

namespace css
{
TEST(SelectorTest, BasicAssertions)
{
	Selector s = {};
	EXPECT_FALSE(s.is_selector_list());
}

// SelectorParseTests

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
		ul)";

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
	std::string css = R"(div.my_class)";

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

TEST(SelectorParseTests, ParseSimpleAttribute)
{
	std::string css = R"([href])";

	auto selector = Parser::parse_selector(css);

	EXPECT_FALSE(selector->is_selector_list());
	EXPECT_FALSE(selector->is_compound());
	EXPECT_EQ(selector->complex_selectors.size(), 1);

	auto complex = selector->complex_selectors[0];
	EXPECT_EQ(complex.compound_selectors.size(), 1);

	auto compound = complex.compound_selectors[0];
	EXPECT_EQ(compound.simple_selectors.size(), 1);
	EXPECT_EQ(compound.combinator, Selector::Combinator::None);

	auto simple_selector = compound.simple_selectors[0];
	EXPECT_TRUE(simple_selector.is_attribute_selector);
	EXPECT_EQ(simple_selector.type, Selector::SimpleSelector::Type::Attribute);

	auto attribute_selctor = simple_selector.attribute_selctor;
	EXPECT_EQ(attribute_selctor.matcher_type, Selector::SimpleSelector::AttributeSelector::MatcherType::None);
	EXPECT_EQ(attribute_selctor.name, "href");
	EXPECT_EQ(attribute_selctor.matcher, "");
	EXPECT_EQ(attribute_selctor.modifier, "");
}

TEST(SelectorParseTests, ParseSimpleAttributeWithMatcher)
{
	std::string css = R"([href="www.kravitz.dev"])";

	auto selector = Parser::parse_selector(css);

	EXPECT_FALSE(selector->is_selector_list());
	EXPECT_FALSE(selector->is_compound());
	EXPECT_EQ(selector->complex_selectors.size(), 1);

	auto complex = selector->complex_selectors[0];
	EXPECT_EQ(complex.compound_selectors.size(), 1);

	auto compound = complex.compound_selectors[0];
	EXPECT_EQ(compound.simple_selectors.size(), 1);
	EXPECT_EQ(compound.combinator, Selector::Combinator::None);

	auto simple_selector = compound.simple_selectors[0];
	EXPECT_TRUE(simple_selector.is_attribute_selector);
	EXPECT_EQ(simple_selector.type, Selector::SimpleSelector::Type::Attribute);

	auto attribute_selctor = simple_selector.attribute_selctor;
	EXPECT_EQ(attribute_selctor.matcher_type, Selector::SimpleSelector::AttributeSelector::MatcherType::None);
	EXPECT_EQ(attribute_selctor.name, "href");
	EXPECT_EQ(attribute_selctor.matcher, "=");
	EXPECT_EQ(attribute_selctor.modifier, "www.kravitz.dev");
}

TEST(SelectorParseTests, ParseCompoundWithAttribute)
{
	std::string css = R"(#a[href="www.kravitz.dev"])";

	auto selector = Parser::parse_selector(css);

	EXPECT_FALSE(selector->is_selector_list());
	EXPECT_TRUE(selector->is_compound());
	EXPECT_EQ(selector->complex_selectors.size(), 1);

	auto complex = selector->complex_selectors[0];
	EXPECT_EQ(complex.compound_selectors.size(), 1);

	auto compound = complex.compound_selectors[0];
	EXPECT_EQ(compound.simple_selectors.size(), 2);
	EXPECT_EQ(compound.combinator, Selector::Combinator::None);

	auto id_simple_selector = compound.simple_selectors[0];
	EXPECT_EQ(id_simple_selector.type, Selector::SimpleSelector::Type::Id);
	EXPECT_EQ(id_simple_selector.value, "a");

	auto attribute_simple_selector = compound.simple_selectors[1];
	EXPECT_TRUE(attribute_simple_selector.is_attribute_selector);
	EXPECT_EQ(attribute_simple_selector.type, Selector::SimpleSelector::Type::Attribute);

	auto attribute_selctor = attribute_simple_selector.attribute_selctor;
	EXPECT_EQ(attribute_selctor.matcher_type, Selector::SimpleSelector::AttributeSelector::MatcherType::None);
	EXPECT_EQ(attribute_selctor.name, "href");
	EXPECT_EQ(attribute_selctor.matcher, "=");
	EXPECT_EQ(attribute_selctor.modifier, "www.kravitz.dev");
}

// SelectorMatchTests

TEST(SelectorMatchTests, MatchCompound)
{
	auto *element = new Element("div");
	element->add_attribute("class", "class1 class2");
	auto styled_node = StyledNode(element);

	std::string css = R"(div.class2)";
	auto selector = Parser::parse_selector(css);

	EXPECT_TRUE(selector->matches(styled_node));
}

/**
* Tests that this selector:
* div.parent span
*
* matches this html snippet:
*
* <div class="parent">
*   <span>
* </div>
*
*/
TEST(SelectorMatchTests, MatchComplex)
{
	auto div = create_element("div");
	div->add_attribute("class", "parent");
	auto div_styled_node = std::make_shared<StyledNode>(div.get());

	auto span = create_element("span");
	auto styled_node = std::make_shared<StyledNode>(span.get());
	div_styled_node->add_child(styled_node);

	std::string css = R"(div.parent span)";
	auto selector = Parser::parse_selector(css);

	EXPECT_TRUE(selector->matches(*styled_node));
}
}
