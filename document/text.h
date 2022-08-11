#pragma once

#include <string>

#include "node.h"

struct Text : public Node
{
public:
	Text(std::string);
	Text(char);

	void append(char);
	void print(int depth = 0) const ;

	inline NodeType type() const { return NodeType::Text; }
	inline std::string text() const { return m_text; }

private:
	std::string m_text;
};
