#include "object.h"

#include <sstream>

#include "function.h"
#include <iostream>

namespace js
{
Value Object::get(std::string const &key)
{
	// found the key in the properties map
	if (is_defined(key))
		return properties[key];

	// search up the prototype chain for the key
	auto *proto = prototype();
	while (proto)
	{
		if (proto->is_defined(key))
			return proto->properties[key];

		proto = proto->prototype();
	}

	// key not found anywhere in chain, return undefined
	return Value();
}

void Object::set(std::string key, Value value)
{
	properties[key] = value;
}

void Object::set_native(const std::string &name, const std::function<Value(std::vector<Value>)> &fn)
{
	properties[name] = Value(new NativeFunction(fn));
}

bool Object::is_defined(std::string const &key) const
{
	return properties.find(key) != properties.end();
}

Object *Object::prototype() { return ObjectPrototype::the(); }

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

ObjectPrototype::ObjectPrototype() { }

ObjectPrototype *ObjectPrototype::the()
{
	if (instance == nullptr)
		instance = new ObjectPrototype;

	return instance;
}
}
