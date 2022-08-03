#pragma once

#include <string>

#include "node.h"

struct Element : public Node
{
public:
	Element(std::string);
	inline NodeType type() { return NodeType::Element; }
	void print(int depth = 0);

private:
	std::string tag;
	std::unordered_map<std::string, std::string> attrs;
};
