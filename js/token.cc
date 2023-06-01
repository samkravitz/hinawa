#include "token.h"

#include <fmt/format.h>

namespace js
{
Token::Token() :
    Token("", TOKEN_EOF, 0, 0)
{ }

Token::Token(std::string value, TokenType type, int line, int col) :
    m_value(value),
    m_type(type),
    m_line(line),
    m_col(col)
{ }

std::string Token::to_string()
{
	return fmt::format("{{ value: {}, type: {}, line: {}, col: {} }}", m_value, m_type, m_line, m_col);
}
}
