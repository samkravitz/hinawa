#pragma once

#include <memory>
#include <string>

#include "node.h"

class Document
{
public:
	Document(std::shared_ptr<Node>);
	Document();

	void print(std::string const &title) const { m_root->print(title); }
	inline std::shared_ptr<Node> root() const { return m_root; }
	std::shared_ptr<Node> get_body() const;
	std::string get_style() const;

private:
	std::shared_ptr<Node> m_root;
};
