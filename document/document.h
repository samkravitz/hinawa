#pragma once

#include <memory>

#include "node.h"

class Document
{
public:
	Document(std::shared_ptr<Node>);
	Document();

	inline std::shared_ptr<Node> root() const { return m_root; }

private:
	std::shared_ptr<Node> m_root;
};
