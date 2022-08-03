#pragma once

#include <string>

#include "node.h"

struct Text : public Node
{
public:
	Text(std::string);
	Text(char);

	inline NodeType type() { return NodeType::Text; }
	void append(char);
	void print(int depth = 0);

private:
	std::string text;
};
