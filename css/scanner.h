#pragma once

#undef yyFlexLexer
#define yyFlexLexer cssFlexLexer
#include <FlexLexer.h>

#include <sstream>
#include <string>

#include "token.h"

namespace css
{
class Scanner
{
public:
	Scanner(const char *);
	~Scanner();

	Token next();

private:
	std::string source;
	std::istringstream istream;
	FlexLexer *lexer;
};
}
