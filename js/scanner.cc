#include "scanner.h"

namespace js
{
Scanner::Scanner(const char *src) :
    source(src),
    istream(src)
{
	lexer = new yyFlexLexer(&istream);
}

Scanner::~Scanner()
{
	delete lexer;
}

Token Scanner::next()
{
	int tok = lexer->yylex();
	Token t{ lexer->YYText(), static_cast<TokenType>(tok), lexer->lineno(), 0 };
	return t;
}
}
