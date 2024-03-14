#include "styled_node.h"

#include "css.h"
#include "document/element.h"
#include "document/text.h"
#include "parser.h"
#include "stylesheet.h"

#include <cassert>

namespace css
{
/**
 * list of all property names which get inherited from their parent
 * 
 * @ref https://www.w3.org/TR/CSS21/propidx.html
*/
static std::string INHERITED_PROPERTIES[] = {
    "azimuth",
    "border-collapse",
    "border-spacing",
    "caption-side",
    "color",
    "cursor",
    "direction",
    "elevation",
    "empty-cells",
    "font-family",
    "font-size",
    "font-style",
    "font-variant",
    "font-weight",
    "font",
    "letter-spacing",
    "line-height",
    "list-style-image",
    "list-style-position",
    "list-style-type",
    "list-style",
    "orphans",
    "pitch-range",
    "pitch",
    "quotes",
    "richness",
    "speak-header",
    "speak-numeral",
    "speak-punctuation",
    "speak",
    "speech-rate",
    "stress",
    "text-align",
    "text-decoration",    // TODO - remove this
    "text-indent",
    "text-transform",
    "visibility",
    "voice-family",
    "volume",
    "white-space",
    "widows",
    "word-spacing",
};

std::shared_ptr<StyledNode> build_style_tree(const Document &document)
{
	auto default_stylesheet = read_default_stylesheet();
	auto body = document.get_body();
	auto user_stylesheet = Parser::parse_stylesheet(document.get_style());
	std::vector<Stylesheet> stylesheets;
	stylesheets.push_back(default_stylesheet);
	stylesheets.push_back(user_stylesheet);
	auto node = std::make_shared<StyledNode>(body);

	node->preorder([&](auto *n) {
		if (auto *p = n->parent())
			n->inherit_properties(*p);

		if (auto *element = dynamic_cast<Element *>(n->node()))
		{
			for (const auto &stylesheet : stylesheets)
				stylesheet.style(n);

			if (element->has_attribute("style"))
			{
				//for (auto decl : Parser::parse_inline(element->get_attribute("style")))
				//	m_values[decl.name] = decl.value;
			}
		}
	});

	return node;
}

StyledNode::StyledNode(Node *node) :
    m_node(node)
{
	node->for_each_child([&](auto *child) {
		if (auto *text = dynamic_cast<Text *>(child))
		{
			if (text->whitespace_only())
				return;
		}

		add_child(std::make_shared<StyledNode>(child));
	});
}

Value *StyledNode::property(const std::string &property_name) const
{
	if (m_values.find(property_name) != m_values.end())
		return m_values.at(property_name);

	return initial_value(property_name);
}

std::string StyledNode::string_or_fallback(const std::string &property_name, const std::string &fallback) const
{
	if (m_values.find(property_name) == m_values.end())
		return fallback;

	auto *value = m_values.at(property_name);
	auto *keyword = dynamic_cast<css::Keyword *>(value);
	if (!keyword)
		return fallback;

	return keyword->value;
}

void StyledNode::inherit_properties(const StyledNode &parent)
{
	for (const auto &property : INHERITED_PROPERTIES)
	{
		m_values[property] = parent.property(property);
	}
}

Display StyledNode::display()
{
	auto *display = property("display");
	auto *keyword = dynamic_cast<Keyword *>(display);

	if (keyword->value == "block" || keyword->value == "table")
		return Display::Block;

	if (keyword->value == "inline")
		return Display::Inline;

	if (keyword->value == "inline-block")
		return Display::InlineBlock;

	if (keyword->value == "list-item")
		return Display::ListItem;

	if (keyword->value == "none")
		return Display::None;

	return Display::Inline;
}

void StyledNode::assign(const std::string &name, Value *value)
{
	ValueArray *value_array = dynamic_cast<ValueArray *>(value);
	if (value_array)
	{
		if (name == "margin")
		{
			const auto &values = value_array->values;
			assert(!values.empty() && values.size() <= 4);

			// if the margin has 1 value, all 4 margins are that value
			if (values.size() == 1)
			{
				m_values["margin-top"] = values[0];
				m_values["margin-right"] = values[0];
				m_values["margin-bottom"] = values[0];
				m_values["margin-left"] = values[0];
			}

			/**
			* If the margin has 2 values:
			* top and bottom margins are the first value
			* left and right margins are the second value
			*/
			if (values.size() == 2)
			{
				m_values["margin-top"] = values[0];
				m_values["margin-right"] = values[1];
				m_values["margin-bottom"] = values[0];
				m_values["margin-left"] = values[1];
			}

			/**
			* If the margin has 3 values:
			* top margin is the first value
			* left and right margins are the second value
			* bottom margin is the third value
			*/
			if (values.size() == 2)
			{
				m_values["margin-top"] = values[0];
				m_values["margin-right"] = values[1];
				m_values["margin-bottom"] = values[2];
				m_values["margin-left"] = values[1];
			}

			/**
			* If the margin has 4 values:
			* top margin is the first value
			* right margin is the second value
			* bottom margin is the third value
			* left margin is the fourth value
			*/
			if (values.size() == 4)
			{
				m_values["margin-top"] = values[0];
				m_values["margin-right"] = values[1];
				m_values["margin-bottom"] = values[2];
				m_values["margin-left"] = values[3];
			}

			return;
		}

		if (name == "padding")
		{
			const auto &values = value_array->values;
			assert(!values.empty() && values.size() <= 4);

			// if the padding has 1 value, all 4 paddings are that value
			if (values.size() == 1)
			{
				m_values["padding-top"] = values[0];
				m_values["padding-right"] = values[0];
				m_values["padding-bottom"] = values[0];
				m_values["padding-left"] = values[0];
			}

			/**
			* If the padding has 2 values:
			* top and bottom paddings are the first value
			* left and right paddings are the second value
			*/
			if (values.size() == 2)
			{
				m_values["padding-top"] = values[0];
				m_values["padding-right"] = values[1];
				m_values["padding-bottom"] = values[0];
				m_values["padding-left"] = values[1];
			}

			/**
			* If the padding has 3 values:
			* top padding is the first value
			* left and right paddings are the second value
			* bottom padding is the third value
			*/
			if (values.size() == 2)
			{
				m_values["padding-top"] = values[0];
				m_values["padding-right"] = values[1];
				m_values["padding-bottom"] = values[2];
				m_values["padding-left"] = values[1];
			}

			/**
			* If the padding has 4 values:
			* top padding is the first value
			* right padding is the second value
			* bottom padding is the third value
			* left padding is the fourth value
			*/
			if (values.size() == 4)
			{
				m_values["padding-top"] = values[0];
				m_values["padding-right"] = values[1];
				m_values["padding-bottom"] = values[2];
				m_values["padding-left"] = values[3];
			}

			return;
		}

		if (name == "border-width")
		{
			const auto &values = value_array->values;
			assert(!values.empty() && values.size() <= 4);

			// if the border-width has 1 value, all 4 border-widths are that value
			if (values.size() == 1)
			{
				m_values["border-top-width"] = values[0];
				m_values["border-right-width"] = values[0];
				m_values["border-bottom-width"] = values[0];
				m_values["border-left-width"] = values[0];
			}

			/**
			* If the border-width has 2 values:
			* top and bottom border-widths are the first value
			* left and right border-widths are the second value
			*/
			if (values.size() == 2)
			{
				m_values["border-top-width"] = values[0];
				m_values["border-right-width"] = values[1];
				m_values["border-bottom-width"] = values[0];
				m_values["border-left-width"] = values[1];
			}

			/**
			* If the border-width has 3 values:
			* top border-width is the first value
			* left and right border-widths are the second value
			* bottom border-width is the third value
			*/
			if (values.size() == 2)
			{
				m_values["border-top-width"] = values[0];
				m_values["border-right-width"] = values[1];
				m_values["border-bottom-width"] = values[2];
				m_values["border-left-width"] = values[1];
			}

			/**
			* If the border-width has 4 values:
			* top border-width is the first value
			* right border-width is the second value
			* bottom border-width is the third value
			* left border-width is the fourth value
			*/
			if (values.size() == 4)
			{
				m_values["border-top-width"] = values[0];
				m_values["border-right-width"] = values[1];
				m_values["border-bottom-width"] = values[2];
				m_values["border-left-width"] = values[3];
			}

			return;
		}
	}

	m_values[name] = value;
}
}
