#pragma once

#include <string>

#include "node.h"

struct Text : public Node
{
public:
	Text(std::string);
	Text(char);

	inline NodeType type() const { return NodeType::Text; }
	inline std::string text() const { return m_text; }

	void append(char);
	void print(int depth = 0) const;
	bool whitespace_only() const;

private:
	std::string m_text;
};
