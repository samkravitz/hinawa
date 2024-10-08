#include "document.h"
#include "element.h"
#include "js/vm.h"
#include "node.h"
#include "text.h"

Document::Document(std::shared_ptr<Node> root) :
    m_root(root)
{ }

Document::Document(const Url &origin) :
    m_origin(origin)
{ }

void Document::print(std::string const &title) const
{
	m_root->print(title);
}

Element *Document::get_element_by_id(const std::string &id)
{
	auto *element = m_root->first_child_that_matches_condition([&](auto *child) {
		if (child->type() == NodeType::Element)
		{
			auto *element = static_cast<const Element *>(child);
			if (!element->has_attribute("id"))
				return false;

			return element->get_attribute("id") == id;
		}

		return false;
	});

	if (!element)
		return nullptr;

	return static_cast<Element *>(element);
}

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
			auto *text_element = dynamic_cast<Text *>(child->last_child());
			style_text = text_element->text();
		}
	});
	return style_text;
}

// returns the source code of the javascript in the document, if any
std::string Document::get_script() const
{
	std::string source = "";
	m_root->preorder([&source](auto *child) {
		if (child->element_name() == "script")
		{
			auto *text_element = dynamic_cast<Text *>(child->last_child());
			source = text_element->text();
		}
	});
	return source;
}

void Document::execute_script_node(std::shared_ptr<Node> node)
{
	auto script_element = std::dynamic_pointer_cast<Element>(node);
	auto text_element = dynamic_cast<Text *>(script_element->last_child());
	auto text = text_element->text();
	vm().interpret(text);
}

void Document::add_child(const std::shared_ptr<Node> &child)
{
	if (!m_root)
		m_root = child;
	else
		m_root->add_child(child);
}

void Document::set_alert(const std::string &text)
{
	m_show_alert = true;
	m_alert_text = text;
}

js::Vm &Document::vm()
{
	if (!m_vm)
		m_vm = new js::Vm(this);
	return *m_vm;
}
