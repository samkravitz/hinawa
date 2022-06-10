#pragma once

#include <list>
#include <sstream>
#include <string>

#include <FlexLexer.h>

#include "token.h"

namespace js
{
class Scanner
{
public:
	Scanner(const char *);
	~Scanner();
	void scan();
	std::string to_string();

private:
	std::list<Token> tokens;
	std::string source;
	std::istringstream istream;
	FlexLexer *lexer;
};
}
