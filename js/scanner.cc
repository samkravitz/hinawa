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
	int tok, col = 1;
	if ((tok = lexer->yylex()))
		return Token { lexer->YYText(), static_cast<TokenType>(tok), lexer->lineno(), col };
	else
		return Token { "EOF", TOKEN_EOF, lexer->lineno(), col };
}
}
