#pragma once

#include <string>

#include "node.h"

struct Element : public Node
{
public:
	Element(std::string);
	inline NodeType type() { return NodeType::Element; }
	inline std::string tag() { return m_tag; }

	void print(int depth = 0);

private:
	std::string m_tag;
	std::unordered_map<std::string, std::string> attrs;
};
