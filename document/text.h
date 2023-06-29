#pragma once

#include <string>

#include "node.h"

struct Text : public Node
{
public:
	Text(Document &, std::string);
	Text(std::string);
	Text(char);

	inline NodeType type() const { return NodeType::Text; }
	inline std::string text() const { return m_text; }

	void append(char);
	bool whitespace_only() const;
	std::string text_for_rendering();
	std::string to_string() const;

private:
	std::string m_text;
};
