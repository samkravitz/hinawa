#pragma once

#undef yyFlexLexer
#define yyFlexLexer cssFlexLexer
#include <FlexLexer.h>

#include <sstream>
#include <string>
#include <vector>

#include "token.h"

namespace css
{
class Scanner
{
public:
	Scanner(const char *);
	~Scanner();

	Token next();
	std::vector<Token> tokenize();

private:
	std::string source;
	std::istringstream istream;
	FlexLexer *lexer;
};
}
