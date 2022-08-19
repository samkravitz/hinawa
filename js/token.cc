#include "token.h"

#include <sstream>

namespace js
{
Token::Token(std::string value, TokenType type, int line, int col) :
    m_value(value),
    m_type(type),
    m_line(line),
    m_col(col)
{ }

std::string Token::to_string()
{
	std::stringstream str;
	str << "{";
	str << " value: " << m_value;
	str << ", type: " << m_type;
	str << ", line: " << m_line;
	str << ", col: " << m_col;
	str << " }";
	return str.str();
}
}
