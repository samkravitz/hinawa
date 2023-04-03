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

	std::cout << buffer.str() << "\n";

	auto parser = js::Parser(buffer.str());
	auto stmts = parser.parse();
	auto printer = js::AstPrinter{};
	printer.print(stmts);

	js::Compiler compiler{stmts};
	auto fn = compiler.compile();
	js::Vm vm{};
	vm.run(fn);
}
