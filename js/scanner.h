#pragma once

#include <sstream>
#include <string>

#undef yyFlexLexer
#define yyFlexLexer jsFlexLexer
#include <FlexLexer.h>

#include "token.h"

namespace js
{
class Scanner
{
public:
	Scanner(const char *);
	~Scanner();

	Token next();
	std::string to_string();

private:
	std::string source;
	std::istringstream istream;
	FlexLexer *lexer;
};
}
