#include "ast/ast.h"
#include "ast_printer.h"
#include "interpreter.h"
#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ./main [script]\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf();

	js::Parser parser(buffer.str());
	auto ast = parser.parse();

	js::AstPrinter printer;
	printer.print(ast);
	std::cout << '\n';

	js::Interpreter interpreter;
	interpreter.run(ast);
}
