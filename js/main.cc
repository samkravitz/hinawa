#include <fstream>
#include <iostream>
#include <sstream>

#include "compiler.h"
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

	auto compiler = js::Compiler(buffer.str().c_str());
	auto fn = compiler.compile();

	#ifdef DEBUG_PRINT_CODE
	fn.chunk.disassemble("script");
	#endif

	auto vm = js::Vm{};
	vm.run(fn);
}
