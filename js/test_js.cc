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

	auto read_file = [](fs::path p) -> std::string {
		std::ifstream stream(p);
		std::ostringstream sstr;
		sstr << stream.rdbuf();
		return sstr.str();
	};

	auto harness = read_file(new_tests_path / "harness.js");
	auto afterthought = read_file(new_tests_path / "afterthought.js");

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

		program << harness;
		program << test.rdbuf();
		program << afterthought;

		js::Vm vm{};
		vm.interpret(program.str());
		fmt::print("\n");
	}
}
