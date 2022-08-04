#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "html/parser.h"
#include "render/window.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ./hinawa [url]\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf();

	auto parser = html::Parser(buffer.str());
	auto document = parser.parse();
	document->print();

	Window window(document);
	return 0;
}
