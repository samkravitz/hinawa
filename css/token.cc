#include "token.h"

#include <sstream>

namespace css
{
Token::Token()
{
	m_value = "eof";
	m_type = Eof;
	m_line = 0;
}

Token::Token(std::string value, TokenType type, int line) :
    m_value(value),
    m_type(type),
    m_line(line)
{ }

std::string Token::to_string()
{
	std::stringstream str;
	str << "{";
	str << " value: " << m_value;
	str << ", type: " << m_type;
	str << ", line: " << m_line;
	str << " }";
	return str.str();
}

}
