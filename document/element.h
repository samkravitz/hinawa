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
	void add_attribute(std::string, std::string);
	bool has_attribute(std::string) const;
	std::string get_attribute(std::string);

private:
	std::string m_tag;
	std::unordered_map<std::string, std::string> attrs;
};
