#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "vm.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
	fs::path new_tests_path(TEST_DIR);
	std::ifstream harness(new_tests_path / "harness.js");
	std::ifstream afterthought(new_tests_path / "afterthought.js");

	for (const auto &entry : fs::recursive_directory_iterator(new_tests_path / "tests"))
	{
		fs::path path(entry);
		if (fs::is_directory(path))
			continue;

		std::string filename = path.filename();
		auto relative_path = fs::relative(path, new_tests_path / "tests").generic_string();
		fmt::print("[TESTING] {}\n", relative_path);

		std::stringstream program;
		std::ifstream test(path);

		program << harness.rdbuf();
		program << test.rdbuf();
		program << afterthought.rdbuf();

		js::Vm vm{};
		vm.interpret(program.str());
	}
}
