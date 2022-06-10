#include "scanner.h"

// https://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_node/flex_19.html
int magic();

namespace js
{

Scanner::Scanner(const char *src) :
	istream(src),
	source(src)
{
	lexer = new yyFlexLexer(&istream);
}

Scanner::~Scanner()
{
	delete lexer;
}

void Scanner::scan()
{
	int tok, col = 1;
	while (tok = lexer->yylex())
	{
		tokens.push_back({ lexer->YYText(), static_cast<TokenType>(tok), lexer->lineno(), col });
		col += lexer->YYLeng();
	}
}

std::string Scanner::to_string()
{
	std::string str = "Scanner: [\n";
	for (auto &tok : tokens)
	{
		str += '\t' + tok.to_string() + "\n";
	}
	str += "]";
	return str;
}

}
