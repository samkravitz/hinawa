#include "object.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include "array.h"
#include "function.h"
#include "object_string.h"
#include "string.hh"
#include "vm.h"

namespace js
{
Value Object::get(const String &primitive_string)
{
	return get(primitive_string.string());
}

void Object::set(const String &key, Value value)
{
	set(key.string(), value);
}

Value Object::get(const std::string &key)
{
	// found the key in the properties map
	if (has_own_property(key))
		return properties[key];

	// search up the prototype chain for the key
	auto *proto = prototype();
	while (proto)
	{
		if (proto->has_own_property(key))
			return proto->properties[key];

		proto = proto->prototype();
	}

	// key not found anywhere in chain, return undefined
	return {};
}

void Object::set(const std::string &key, Value value)
{
	properties[key] = value;
}

void Object::set_native(const std::string &name, const std::function<Value(Vm &, const std::vector<Value> &)> &fn)
{
	properties[name] = Value(NativeFunction::create(fn));
}

bool Object::has_own_property(const std::string &key) const
{
	return properties.find(key) != properties.end();
}

bool Object::has_own_property(const String &primitive_string) const
{
	return has_own_property(primitive_string.string());
}

Object *Object::prototype()
{
	if (!m_prototype)
		return ObjectPrototype::the();
	return m_prototype;
}

Function *Object::as_function()
{
	assert(is_function());
	return static_cast<Function *>(this);
}

NativeFunction *Object::as_native()
{
	assert(is_native());
	return static_cast<NativeFunction *>(this);
}

Closure *Object::as_closure()
{
	assert(is_closure());
	return static_cast<Closure *>(this);
}

Array *Object::as_array()
{
	assert(is_array());
	return static_cast<Array *>(this);
}

Value Object::ordinary_to_primitive(const Value::Type &hint) const
{
	// 1. If hint is string, then
	// 		a. Let methodNames be « "toString", "valueOf" ».
	// 2. Else,
	//		a. Let methodNames be « "valueOf", "toString" ».
	// 3. For each element name of methodNames, do
	//		a. Let method be ? Get(O, name).
	//		b. If IsCallable(method) is true, then
	//			i. Let result be ? Call(method, O).
	//			ii. If result is not an Object, return result.
	// 4. Throw a TypeError exception.
	return {};
}

std::string Object::to_string() const
{
	std::stringstream stream;
	stream << "{";

	for (auto it = properties.begin(); it != properties.end(); it++)
	{
		stream << " " << it->first;
		stream << ": ";

		if (it->second.as_object() == this)
			stream << "[Object object]";
		else
			stream << it->second.to_string();

		if (std::next(it) != properties.end())
			stream << ",";
		else
			stream << " ";
	}

	stream << "}";
	return stream.str();
}

void Object::print_prototype_chain()
{
	for (auto *proto = prototype(); proto; proto = proto->prototype())
		fmt::print("{} -> \n", proto->to_string());

	fmt::print("-> null\n");
}

ObjectPrototype *ObjectPrototype::instance = nullptr;

ObjectPrototype::ObjectPrototype()
{
	set_native("hasOwnProperty", [](auto &vm, const auto &argv) -> Value {
		if (argv.empty())
			return Value(false);

		if (!argv[0].is_string())
			return Value(false);

		auto *obj = vm.current_this();

		const auto &property = argv[0].as_string();
		return Value(obj->has_own_property(property));
	});

	set_native("getPrototypeOf", [](auto &vm, const auto &argv) -> Value {
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
}

ObjectPrototype *ObjectPrototype::the()
{
	if (instance == nullptr)
		instance = heap().allocate<ObjectPrototype>();

	return instance;
}
}
