#include "prelude.h"

#include <fmt/format.h>

#include "object_string.h"
#include "vm.h"

namespace js
{
/**
* prelude for the global Object object in javascript.
* adds methods like Object.getPrototypeOf()
*/
static Object *prelude_object(Vm &vm)
{
	auto *object = new Object();
	object->set_native("getPrototypeOf", [](auto &vm, const auto &argv) -> Value {
		// TODO - this should throw, instead of returning undefined
		if (argv.empty())
			return {};

		auto obj = argv[0];
		if (obj.is_string())
			return Value(StringPrototype::the());

		// TODO - this should throw, instead of returning undefined
		if (!obj.is_object())
			return {};

		return Value(obj.as_object()->prototype());
	});

	return object;
}

void prelude(Vm &vm)
{
	// create the global object and put some functions on it
	auto *global = new Object();
	global->set("window", Value(global));

	global->set_native("print", [](auto &vm, const auto &argv) -> Value {
		if (argv.empty())
			return {};

		for (uint i = 0; i < argv.size(); i++)
		{
			fmt::print("{}", argv[i].to_string());
			if (i != argv.size() - 1)
				fmt::print(" ");
		}

		fmt::print("\n");
		return {};
	});

	auto *console = new Object();
	console->set_native("log", [](auto &vm, const auto &argv) -> Value {
		if (argv.empty())
			return {};

		for (uint i = 0; i < argv.size(); i++)
		{
			fmt::print("{}", argv[i].to_string());
			if (i != argv.size() - 1)
				fmt::print(" ");
		}

		fmt::print("\n");
		return {};
	});

	global->set("console", Value(console));

	auto *object = prelude_object(vm);
	global->set("Object", Value(object));

	vm.set_global(global);
}
}