#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "parser.h"

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
	ast->print("AST");
}
