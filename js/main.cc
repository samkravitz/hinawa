#include <iostream>
#include <fstream>
#include <vector>

#include "scanner.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ./main [script]\n";
		return 1;
	}

	std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(buffer.data(), size);

	js::Scanner scanner(buffer.data());
	scanner.scan();
	std::cout << scanner.to_string() << "\n";
}
