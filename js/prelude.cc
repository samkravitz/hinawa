#include "prelude.h"

#include <fmt/format.h>

#include "array.h"
#include "bindings/document_wrapper.h"
#include "date.h"
#include "document/document.h"
#include "error.h"
#include "function.h"
#include "heap.h"
#include "object.h"
#include "object_string.h"
#include "value.h"
#include "vm.h"

namespace js
{
/**
* prelude for the global Object object in javascript.
* adds methods like Object.getPrototypeOf()
*/
static void prelude_object(Vm &vm)
{
	auto *object = NativeFunction::create([](auto &vm, const auto &argv) -> Value { return Value(heap().allocate()); });
	auto val = Value(object);
	vm.global()->set("Object", val);

	object->set("prototype", Value(ObjectPrototype::the()));
	ObjectPrototype::the()->set("constructor", val);
}

/**
* prelude for the global Array object in javascript.
*/
static void prelude_array(Vm &vm)
{
	auto *array = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		auto *array = heap().allocate<Array>();
		return Value(array);
	});

	vm.global()->set("Array", Value(array));
}

/**
* prelude for the global Date object in javascript.
*/
static void prelude_date(Vm &vm)
{
	auto *date = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		auto *date = heap().allocate<Date>();
		return Value(date);
	});

	vm.global()->set("Date", Value(date));
}

/**
* prelude for the global Error object in javascript.
*/
static void prelude_error(Vm &vm)
{
	auto *error = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		std::string message = "";
		if (!argv.empty())
		{
			auto &maybe_message = argv[0];
			if (maybe_message.is_string())
				message = maybe_message.as_string().string();
		}

		return Value(heap().allocate<Error>(vm, message));
	});

	auto *reference_error = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		fmt::print(stderr, "Warning: Trying to construct ReferenceError. You should not do this\n");
		return Value::js_undefined();
	});

	auto *type_error = NativeFunction::create([](auto &vm, const auto &argv) -> Value {
		fmt::print(stderr, "Warning: Trying to construct TypeError. You should not do this\n");
		return Value::js_undefined();
	});

	vm.global()->set("Error", Value(error));
	vm.global()->set("ReferenceError", Value(reference_error));
	vm.global()->set("TypeError", Value(type_error));
}

/**
* prelude for the Math object in javascript.
*/
static void prelude_math(Vm &vm)
{
	auto *math = NativeFunction::create([](auto &vm, const auto &argv) -> Value { return Value(heap().allocate()); });

	// https://tc39.es/ecma262/#sec-math.sqrt
	// TODO - implement properly
	math->set_native("sqrt", [](auto &vm, const auto &argv) -> Value {
		auto n = argv[0].as_number();
		return Value(std::sqrt(n));
	});

	// https://tc39.es/ecma262/#sec-math.abs
	// TODO - implement properly
	math->set_native("abs", [](auto &vm, const auto &argv) -> Value {
		auto n = argv[0].as_number();
		return Value(std::abs(n));
	});

	auto val = Value(math);
	vm.global()->set("Math", val);

	vm.global()->set("Infinity", Value::js_infinity());
	vm.global()->set("NaN", Value::js_nan());
}

/**
* prelude for the document object in javascript.
*/
static void prelude_document(Vm &vm, Document *document)
{
	auto *document_wrapper = heap().allocate<bindings::DocumentWrapper>(document);
	vm.set_document_wrapper(document_wrapper);
	vm.global()->set("document", Value(document_wrapper));
}

void prelude(Vm &vm, Document *document)
{
	// create the global object and put some functions on it
	auto *global = heap().allocate();
	vm.set_global(global);
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

	auto *console = heap().allocate();
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

	prelude_object(vm);
	prelude_array(vm);
	prelude_date(vm);
	prelude_error(vm);
	prelude_math(vm);

	if (document)
		prelude_document(vm, document);

	global->set_native("alert", [](auto &vm, const auto &argv) -> Value {
		std::string text = "";
		if (!argv.empty())
			text = argv[0].to_string();

		vm.document().set_alert(text);
		return {};
	});

	vm.set_global(global);
}
}