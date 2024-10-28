#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <fmt/color.h>
#include <fmt/format.h>

#include "vm.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
	fs::path spec_dir(SPEC_DIR);

	auto read_file = [](fs::path p) -> std::string {
		std::ifstream stream(p);
		std::ostringstream sstr;
		sstr << stream.rdbuf();
		return sstr.str();
	};

	auto harness = read_file(spec_dir / "harness.js");

	int failed_suites = 0;
	int passed_suites = 0;

	int failed_tests = 0;
	int passed_tests = 0;

	for (const auto &entry : fs::recursive_directory_iterator(spec_dir))
	{
		fs::path path(entry);
		if (fs::is_directory(path))
			continue;

		std::string filename = path.filename();

		if (filename == "harness.js")
			continue;

		auto relative_path = fs::relative(path, spec_dir).generic_string();
		fmt::print("[TESTING] {}\n", relative_path);

		std::stringstream program;
		std::ifstream test(path);

		program << harness;
		program << test.rdbuf();

		js::Vm vm{};
		vm.interpret(program.str());

		auto &global_object = vm.global();
		auto *results = global_object.get(js::String("__TestResults__")).as_object();
		auto properties = results->get_properties();

		for (const auto &[suite_name, suite_details] : results->get_properties())
		{
			bool all_passed = true;

			for (const auto &[test_name, test_details] : suite_details.as_object()->get_properties())
			{
				auto *obj = test_details.as_object();
				if (obj->get("passed").as_bool())
				{
					passed_tests += 1;
				}

				else
				{
					failed_tests += 1;
					all_passed = false;
					auto message = obj->get("details").as_string().string();
					fmt::print(fmt::fg(fmt::color::fire_brick), "{} > {}\n", suite_name, test_name);
					fmt::print("\t{}\n", message);
				}
			}

			if (all_passed)
				passed_suites += 1;
			else
				failed_suites += 1;
		}
	}

	fmt::print("Test suites: ");
	fmt::print(fmt::fg(fmt::color::fire_brick), "{} failed, ", failed_suites);
	fmt::print(fmt::fg(fmt::color::green), "{} passed, ", passed_suites);
	fmt::print("{} total\n", passed_suites + failed_suites);

	fmt::print("Tests      : ");
	fmt::print(fmt::fg(fmt::color::fire_brick), "{} failed, ", failed_tests);
	fmt::print(fmt::fg(fmt::color::green), "{} passed, ", passed_tests);
	fmt::print("{} total\n", passed_tests + failed_tests);

	return failed_suites;
}
