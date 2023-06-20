#pragma once

#include <memory>
#include <string>

#include "web/url.h"

class Node;

class Document
{
public:
	Document() = default;
	Document(std::shared_ptr<Node>);
	Document(const Url &);

	void print(std::string const &) const;
	inline std::shared_ptr<Node> root() const { return m_root; }
	Url origin() const { return m_origin; }
	Node *get_body() const;
	std::string get_style() const;
	std::string get_script() const;
	void add_child(const std::shared_ptr<Node> &);
	inline void set_needs_reflow(bool b = true) { m_needs_reflow = b; }
	inline bool needs_reflow() const { return m_needs_reflow; }

private:
	std::shared_ptr<Node> m_root{ nullptr };
	Url m_origin;
	bool m_needs_reflow{ false };
};
