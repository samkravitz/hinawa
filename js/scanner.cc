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
		return {lexer->YYText(), static_cast<TokenType>(tok), lexer->lineno(), col};
	else
		return {"EOF", TOKEN_EOF, lexer->lineno(), col};
}

std::vector<Token> Scanner::scan()
{
	std::vector<Token> tokens;
	while (Token tok = next())
		tokens.push_back(tok);

	tokens.push_back({"EOF", TOKEN_EOF, 0, 0});
	return tokens;
}
}
