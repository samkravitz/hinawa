#include "styled_node.h"

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
	"text-decoration", // TODO - remove this
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
	auto user_stylesheet = Parser(document.get_style()).parse();
	std::vector<Stylesheet> stylesheets;
	stylesheets.push_back(default_stylesheet);
	stylesheets.push_back(user_stylesheet);
	std::shared_ptr<StyledNode> node = std::make_shared<StyledNode>(body, stylesheets);
	return node;
}

StyledNode::StyledNode(Node *node, const std::vector<Stylesheet> &stylesheets, StyledNode *parent) :
    m_node(node)
{
	if (parent)
		inherit_properties(*parent);

	if (node->type() == NodeType::Element)
	{
		auto *element = dynamic_cast<Element *>(node);

		for (const auto &stylesheet : stylesheets)
		{
			for (auto decl : stylesheet.universal_rules())
				m_values[decl.name] = decl.value;

			for (auto decl : stylesheet.rules_for_tag(element->tag()))
				m_values[decl.name] = decl.value;

			if (element->has_attribute("class"))
			{
				for (auto decl : stylesheet.rules_for_class(element->get_attribute("class")))
					m_values[decl.name] = decl.value;
			}
		}
	}

	node->for_each_child(
	    [this, &stylesheets](auto *child)
	    {
		    // skip text nodes that are only whitespace;
		    // they don't belong in the style tree
		    if (child->type() == NodeType::Text)
		    {
			    auto *text = dynamic_cast<Text *>(child);
			    if (text->whitespace_only())
				    return;
		    }

		    add_child(std::make_shared<StyledNode>(child, stylesheets, this));
	    });
}

Value *StyledNode::lookup(const std::string &property_name, Value *const fallback) const
{
	if (m_values.find(property_name) != m_values.end())
		return m_values.at(property_name);

	return fallback;
}

Value *
StyledNode::lookup(const std::string &property_name1, const std::string &property_name2, Value *const fallback) const
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
		m_values[property] = parent.lookup(property);
	}
}

Display StyledNode::display()
{
	auto inline_display = css::Keyword{ "inline" };

	auto *display = lookup("display", &inline_display);
	auto *keyword = dynamic_cast<Keyword *>(display);

	if (keyword->value == "block")
		return Display::Block;

	if (keyword->value == "inline")
		return Display::Inline;

	if (keyword->value == "none")
		return Display::None;

	return Display::Inline;
}
}
