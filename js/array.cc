#include "array.h"

#include <sstream>

#include "vm.h"

namespace js
{
Array::Array() :
    Array(std::vector<Value>{})
{ }

Array::Array(std::vector<Value> array)
{
	for (auto element : array)
		push_back(element);

	set("length", Value((double) size()));
}

Object *Array::prototype()
{
	return ArrayPrototype::the();
}

ArrayPrototype *ArrayPrototype::instance = nullptr;

ArrayPrototype::ArrayPrototype()
{
	set_native("push", [](auto &vm, const auto &argv) -> Value {
		auto *arr = vm.current_this()->as_array();
		for (const auto &val : argv)
			arr->push_back(val);

		auto len = Value((double) arr->size());
		arr->set("length", len);
		return len;
	});

	set_native("map", [](auto &vm, const auto &argv) -> Value {
		auto *arr = vm.current_this()->as_array();
		auto callback = argv[0].as_object()->as_closure();

		auto *new_arr = heap().allocate<Array>();
		for (const auto &val : *arr)
		{
			vm.push(val);
			vm.call(callback);
			auto res = vm.pop();
			new_arr->push_back(res);
		}

		return Value(new_arr);
	});
}

ArrayPrototype *ArrayPrototype::the()
{
	if (!instance)
		instance = heap().allocate<ArrayPrototype>();

	return instance;
}

std::string Array::to_string() const
{
	std::stringstream stream;
	std::vector<Value> vec = *this;
	stream << "[";
	for (unsigned i = 0; i < vec.size(); i++)
	{
		stream << vec[i].to_string();
		if (i != vec.size() - 1)
			stream << ", ";
	}
	stream << "]";
	return stream.str();
}
}