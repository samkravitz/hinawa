#include "styled_node.h"

#include "css.h"
#include "document/element.h"
#include "document/text.h"
#include "parser.h"
#include "stylesheet.h"

#include <iostream>

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

Value *const default_font_size = new Length(16, Length::PX);

std::shared_ptr<StyledNode> build_style_tree(const Document &document)
{
	auto default_stylesheet = read_default_stylesheet();
	auto body = document.get_body();
	auto user_stylesheet = Parser::parse(document.get_style());
	std::vector<Stylesheet> stylesheets;
	stylesheets.push_back(default_stylesheet);
	stylesheets.push_back(user_stylesheet);
	auto node = std::make_shared<StyledNode>(body);

	node->preorder([&](auto *n) {
		if (auto *p = n->parent())
			n->inherit_properties(*p);

		if (auto *element = dynamic_cast<Element*>(n->node()))
		{
			for (const auto &stylesheet : stylesheets)
				stylesheet.style(n);

			if (element->has_attribute("style"))
			{
				for (auto decl : Parser::parse_inline(element->get_attribute("style")))
					n->assign(decl.name, decl.style_value());
			}
		}
	});

	return node;
}

StyledNode::StyledNode(Node *node) :
    m_node(node)
{
	node->for_each_child([&](auto *child) {
		if (auto *text = dynamic_cast<Text*>(child))
		{
			if (text->whitespace_only())
				return;
		}

		add_child(std::make_shared<StyledNode>(child));
	});
}

Value *StyledNode::property(const std::string &property_name, Value *const fallback) const
{
	if (m_values.find(property_name) != m_values.end())
		return m_values.at(property_name);

	return fallback;
}

Value *
StyledNode::property(const std::string &property_name1, const std::string &property_name2, Value *const fallback) const
{
	if (m_values.find(property_name1) != m_values.end())
		return m_values.at(property_name1);

	if (m_values.find(property_name2) != m_values.end())
		return m_values.at(property_name2);

	return fallback;
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
	auto inline_display = css::Keyword{ "inline" };

	auto *display = property("display", &inline_display);
	auto *keyword = dynamic_cast<Keyword *>(display);

	if (keyword->value == "block")
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
}
