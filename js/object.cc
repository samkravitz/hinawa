#include "object.h"

#include <cassert>
#include <sstream>

#include "array.h"
#include "date.h"
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

void Object::set(const String &key, Value value, int attributes)
{
	set(key.string(), value, attributes);
}

Value Object::get(const std::string &key)
{
	// found the key in the properties map
	if (has_own_property(key))
	{
		auto value = own_properties[key].value;
		if (value.is_object() && value.as_object()->is_native_property())
		{
			auto *native_property = value.as_object()->as_native_property();
			return native_property->get(this);
		}

		return value;
	}

	// search up the prototype chain for the key
	auto *proto = prototype();
	while (proto)
	{
		if (proto->has_own_property(key))
		{
			auto value = proto->own_properties[key].value;
			if (value.is_object() && value.as_object()->is_native_property())
			{
				auto *native_property = value.as_object()->as_native_property();
				return native_property->get(this);
			}

			return value;
		}

		proto = proto->prototype();
	}

	// key not found anywhere in chain, return undefined
	return {};
}

void Object::set(const std::string &key, Value value, int attributes)
{
	bool has_own_prop = has_own_property(key);

	if (has_own_prop)
	{
		auto got_value = own_properties[key].value;

		if (got_value.is_object() && got_value.as_object()->is_native_property())
		{
			auto *native_property = got_value.as_object()->as_native_property();
			native_property->set(this, value);
			return;
		}
	}

	// fail if property is not writable
	if (has_own_prop)
	{
		auto prop = own_properties[key];
		if (!prop.is_writable())
			return;
	}

	own_properties[key] = Property(value, attributes);
}

void Object::set_native(const std::string &name, const std::function<Value(Vm &, const std::vector<Value> &)> &fn)
{
	own_properties[name] = Property(Value(NativeFunction::create(fn)), 0);
}

void Object::set_native_property(const std::string &name,
                                 const std::function<Value(Object *)> &getter,
                                 const std::function<void(Object *, Value)> &setter)
{
	own_properties[name] = Property(Value(NativeProperty::create(getter, setter)), 0);
}

bool Object::has_own_property(const std::string &key) const
{
	return own_properties.find(key) != own_properties.end();
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

NativeProperty *Object::as_native_property()
{
	assert(is_native_property());
	return static_cast<NativeProperty *>(this);
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

Date *Object::as_date()
{
	assert(is_date());
	return static_cast<Date *>(this);
}

const Array *Object::as_array() const
{
	assert(is_array());
	return static_cast<const Array *>(this);
}

Value Object::ordinary_to_primitive(Vm &vm, const Value::Type &hint) const
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

	// TODO - this is not correct
	if (is_array())
	{
		auto *array = as_array();
		std::stringstream stream;

		for (auto it = array->begin(); it != array->end(); it++)
		{
			stream << it->to_string();
			if (std::next(it) != array->end())
				stream << ",";
		}

		return Value(vm.heap().allocate_string(stream.str()));
	}

	return {};
}

std::string Object::to_string() const
{
	std::stringstream stream;
	stream << "{";

	for (auto it = own_properties.begin(); it != own_properties.end(); it++)
	{
		stream << " " << it->first;
		stream << ": ";

		if (it->second.value.as_object() == this)
			stream << "[Object object]";
		else
			stream << it->second.value.to_string();

		if (std::next(it) != own_properties.end())
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
}

ObjectPrototype *ObjectPrototype::the()
{
	if (instance == nullptr)
		instance = heap().allocate<ObjectPrototype>();

	return instance;
}
}
