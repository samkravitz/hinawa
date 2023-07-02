#include "prelude.h"

#include <fmt/format.h>

#include "array.h"
#include "function.h"
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
	auto *object = NativeFunction::create([](auto &vm, const auto &argv) -> Value { return Value(new Object); });

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

/**
* prelude for the global Array object in javascript.
*/
static Object *prelude_array(Vm &vm)
{
	auto *array = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		auto *array = new Array();
		return Value(array);
	});

	return array;
}

/**
* prelude for the document object in javascript.
*/
static Object *prelude_document(Vm &vm)
{
	auto *document = new Object();

	document->set_native("getElementById", [](auto &vm, const auto &argv) -> Value {
		if (argv.empty())
			return Value::js_null();

		auto id = argv[0].to_string();
		return Value(vm.document().get_element_by_id(id));
	});

	return document;
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

	auto *array = prelude_array(vm);
	global->set("Array", Value(array));

	auto *document = prelude_document(vm);
	global->set("document", Value(document));

	global->set_native("alert", [](auto &vm, const auto &argv) -> Value {
		std::string text = "";
		if (!argv.empty())
			text = *argv[0].as_string();

		vm.document().set_alert(text);
		return {};
	});

	vm.set_global(global);
}
}