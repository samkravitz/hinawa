#pragma once

#include <string>

#include "token_type.h"

namespace js
{
class Token
{
public:
	Token() = default;
	Token(std::string, TokenType, int, int);

	inline TokenType type() const { return m_type; }
	inline std::string value() const { return m_value; }
	inline int line() const { return m_line; }
	inline int col() const { return m_col; }

	std::string to_string();

private:
	std::string m_value;
	TokenType m_type;
	int m_line;
	int m_col;
};
}
