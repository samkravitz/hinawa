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
}

ArrayPrototype *ArrayPrototype::the()
{
	if (!instance)
		instance = new ArrayPrototype;

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