#include "token.h"

#include <sstream>

namespace js
{

Token::Token(std::string value, TokenType type, int line, int col) :
	value(value),
	type(type),
	line(line),
	col(col)
{ }

std::string Token::to_string()
{
	std::stringstream str;
	str << "{";
	str << " value: " << value;
	str << ", type: " << type;
	str << ", line: " << line;
	str << ", col: " << col;
	str << " }";
	return str.str();
}

}
