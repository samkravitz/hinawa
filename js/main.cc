#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/wait.h>

#include "vm.h"

namespace fs = std::filesystem;

static constexpr int RESULT_TEST_PASSED = 199;
static constexpr int RESULT_TEST_FAILED = 198;

void test_262_runner(const std::string &test_262_dir)
{
	fs::path test_262_path(test_262_dir);
	if (!fs::exists(test_262_path) || !fs::is_directory(test_262_path))
	{
		fmt::print(stderr, "{} is not a directory!\n", test_262_dir);
		return;
	}

	int passed = 0;
	int failed = 0;
	int crashed = 0;

	for (const auto &entry : fs::recursive_directory_iterator(test_262_path / "test"))
	{
		fs::path path(entry);
		if (fs::is_directory(path))
			continue;

		std::string filename = path.filename();
		if (!filename.ends_with(".js"))
			continue;

		fmt::print("testing {}\n", filename);

		auto pid = fork();
		if (pid == 0)
		{
			std::stringstream buffer;
			std::ifstream assert(test_262_path / "harness" / "assert.js");
			std::ifstream sta(test_262_path / "harness" / "sta.js");
			std::ifstream test_file(path);
			buffer << assert.rdbuf();
			buffer << sta.rdbuf();
			buffer << test_file.rdbuf();

			js::Vm vm{};
			vm.interpret(buffer.str());

			int result = RESULT_TEST_PASSED;
			if (vm.has_error())
				result = RESULT_TEST_FAILED;

			exit(result);
		}

		int status;
		waitpid(pid, &status, 0);

		if (WIFSIGNALED(status))
			crashed += 1;

		else if (WEXITSTATUS(status))
		{
			if (status == RESULT_TEST_FAILED)
				failed += 1;
			else if (status == RESULT_TEST_PASSED)
				passed += 1;
			else
				fmt::print(stderr, "Bad status {}\n", status);
		}
	}

	int total = passed + failed + crashed;
	fmt::print("{} tests run. {} passed, {} failed, {} crashed\n", total, passed, failed, crashed);
}

static void repl()
{
	int line = 0;
	js::Vm vm{};

	fmt::print("JS REPL\n.exit to quit\n");
	for (;;)
	{
		std::string source = "";
		fmt::print("> ");
		std::getline(std::cin, source);

		if (std::cin.eof())
			return;

		if (source == "")
			continue;

		if (source == ".exit")
			break;

		vm.interpret(source);
		auto result = vm.last_evaluated_expression();
		fmt::print("{}\n", result.to_string());
		line++;
	}
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		repl();
		return 0;
	}

	if (std::string(argv[1]) == "--test-262")
	{
		std::string test_262_path = fs::current_path();
		if (argc > 2)
			test_262_path = argv[2];

		test_262_runner(test_262_path);
		return 0;
	}

	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf();

	js::Vm vm{};
	vm.interpret(buffer.str());
}
