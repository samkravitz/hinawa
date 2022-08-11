#pragma once

#include <string>
#include <unordered_map>

#include "node.h"

struct Element : public Node
{
public:
	Element(std::string);
	inline NodeType type() const { return NodeType::Element; }
	inline std::string tag() const { return m_tag; }

	void print(int depth = 0) const;

private:
	std::string m_tag;
	std::unordered_map<std::string, std::string> attrs;
};
