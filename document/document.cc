#include "document.h"
#include "element.h"
#include "node.h"
#include "text.h"

Document::Document(std::shared_ptr<Node> root) :
    m_root(root)
{ }

Document::Document(const Url &origin) :
    m_origin(origin)
{ }

Node *Document::get_body() const
{
	Node *body = nullptr;
	m_root->preorder([&](auto *child) {
		if (child->element_name() == "body")
			body = child;
	});
	return body;
}

std::string Document::get_style() const
{
	std::string style_text = "";
	m_root->preorder([&](auto *child) {
		if (child->element_name() == "style")
		{
			auto *text_element = dynamic_cast<Text*>(child->last_child());
			style_text = text_element->text();
		}
	});
	return style_text;
}

void Document::add_child(const std::shared_ptr<Node> &child)
{
	if (!m_root)
		m_root = child;
	else
		m_root->add_child(child);
}
