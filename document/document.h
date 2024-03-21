#pragma once

#include <memory>
#include <string>

#include "web/url.h"

class Node;
class Element;

namespace js
{
class Vm;
}

class Document
{
public:
	Document() = default;
	Document(std::shared_ptr<Node>);
	Document(const Url &);

	Element *get_element_by_id(const std::string &);

	void print(std::string const &) const;
	inline std::shared_ptr<Node> root() const { return m_root; }
	Url origin() const { return m_origin; }
	Node *get_body() const;
	std::string get_style() const;
	std::string get_script() const;
	void add_child(const std::shared_ptr<Node> &);
	inline void set_needs_reflow(bool b = true) { m_needs_reflow = b; }
	inline bool needs_reflow() const { return m_needs_reflow; }

	inline bool show_alert() const { return m_show_alert; }
	void set_alert(const std::string &);
	void clear_alert() { m_show_alert = false; }
	const std::string &alert_text() const { return m_alert_text; }

	js::Vm &vm();
	void execute_script_node(std::shared_ptr<Node>);

private:
	std::shared_ptr<Node> m_root{nullptr};
	Url m_origin;
	bool m_needs_reflow{false};
	bool m_show_alert{false};
	std::string m_alert_text = "";
	js::Vm *m_vm = nullptr;
};
