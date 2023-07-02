#include <fstream>
#include <iostream>
#include <sstream>

#include "ast_printer.h"
#include "compiler.h"
#include "parser.h"
#include "vm.h"

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

	auto program = js::Parser::parse(buffer.str());

#ifdef DEBUG_PRINT_AST
	auto printer = js::AstPrinter{};
	printer.print(program);
#endif

	auto fn = js::Compiler::compile(program);
	js::Vm vm{};
	vm.run(std::move(fn));
}
