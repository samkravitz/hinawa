#include "styled_node.h"

#include "document/element.h"
#include "document/text.h"
#include "parser.h"
#include "stylesheet.h"

#include <iostream>

namespace css
{
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

StyledNode::StyledNode(std::shared_ptr<Node> node,
                       const std::vector<Stylesheet> &stylesheets,
                       std::unordered_map<std::string, Value *> *parent_values) :
    m_node(node)
{
	if (!parent())
		m_values["font-size"] = default_font_size;

	if (parent_values)
		m_values["font-size"] = parent_values->at("font-size");

	if (node->type() == NodeType::Element)
	{
		auto element = std::dynamic_pointer_cast<Element>(node);

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
	    [this, stylesheets](std::shared_ptr<Node> child)
	    {
		    // skip text nodes that are only whitespace;
		    // they don't belong in the style tree
		    if (child->type() == NodeType::Text)
		    {
			    auto text = std::dynamic_pointer_cast<Text>(child);
			    if (text->whitespace_only())
				    return;
		    }

		    add_child(std::make_shared<StyledNode>(StyledNode(child, stylesheets, &m_values)));
	    });
}

Value *StyledNode::lookup(std::string property_name, Value *const fallback)
{
	if (!this)
		return fallback;

	if (m_values.find(property_name) != m_values.end())
		return m_values[property_name];

	return fallback;
}

Value *StyledNode::lookup(std::string property_name1, std::string property_name2, Value *const fallback)
{
	if (!this)
		return fallback;

	if (m_values.find(property_name1) != m_values.end())
		return m_values[property_name1];

	if (m_values.find(property_name2) != m_values.end())
		return m_values[property_name2];

	return fallback;
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
