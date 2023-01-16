#include "scanner.h"

namespace css
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
	if (int tok = lexer->yylex())
		return { lexer->YYText(), static_cast<TokenType>(tok), lexer->lineno() };
	else
		return { "EOF", static_cast<TokenType>(0), lexer->lineno() };
}

std::vector<Token> Scanner::tokenize()
{
	std::vector<Token> tokens;
	while (auto token = next())
		tokens.push_back(token);
	
	return tokens;
}
}
