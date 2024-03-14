#include <fstream>
#include <iostream>
#include <sstream>

#include <FlexLexer.h>

int main()
{
	std::ifstream file("default.css");
	std::stringstream buffer;
	buffer << file.rdbuf();

	std::istringstream istream(buffer.str());

	FlexLexer *lexer = new yyFlexLexer(&istream);

	int tok;

	while ((tok = lexer->yylex()))
	{
		std::cout << tok << " " << lexer->YYText() << "\n";
	}
}
