#pragma once

#include <string>

#include "token_type.h"

namespace js
{
class Token
{
public:
	Token(std::string, TokenType, int, int);
	std::string to_string();

private:
	std::string value;
	TokenType type;
	int line;
	int col;
};
}
