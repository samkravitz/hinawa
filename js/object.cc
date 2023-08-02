#include "object.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include "array.h"
#include "function.h"
#include "primitive_string.h"
#include "vm.h"

namespace js
{
Value Object::get(const PrimitiveString &primitive_string)
{
	return get(primitive_string.string());
}

void Object::set(const PrimitiveString &key, Value value)
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

bool Object::has_own_property(const PrimitiveString &primitive_string) const
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
}

ObjectPrototype *ObjectPrototype::the()
{
	if (instance == nullptr)
		instance = heap().allocate<ObjectPrototype>();

	return instance;
}
}
