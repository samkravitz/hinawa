#pragma once

#include <string>
#include <unordered_map>

#include "node.h"

struct Element : public Node
{
public:
	Element(Document &, std::string);
	Element(std::string);
	inline NodeType type() const { return NodeType::Element; }
	inline std::string tag() const { return m_tag; }

	virtual void add_attribute(std::string, std::string);
	bool has_attribute(std::string) const;
	std::string get_attribute(std::string);
	std::string to_string() const;

private:
	std::string m_tag;
	std::unordered_map<std::string, std::string> attrs;
};
