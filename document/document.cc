#include "document.h"
#include "element.h"
#include "node.h"
#include "text.h"

Document::Document(std::shared_ptr<Node> root) :
    m_root(root)
{ }

std::shared_ptr<Node> Document::get_body() const
{
	std::shared_ptr<Node> body = nullptr;
	m_root->preorder([&](const auto &child)
	{
		if (child->element_name() == "body")
			body = child;
	});
	return body;
}

std::string Document::get_style() const
{
	std::string style_text = "";
	m_root->preorder([&](const auto &child)
	{
		if (child->element_name() == "style")
		{
			auto text_element = std::dynamic_pointer_cast<Text>(child->last_child());
			style_text = text_element->text();
		}
	});
	return style_text;
}
