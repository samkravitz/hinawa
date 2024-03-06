#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "cell.h"
#include "value.h"

namespace js
{
class Vm;
class Function;
class NativeFunction;
class NativeProperty;
class Closure;
class Array;
class String;
class Date;

class Object : public Cell
{
	friend class Heap;

public:
	virtual ~Object() { }

	Value get(const String &);
	Value get(const std::string &);
	void set(const String &, Value);
	void set(const std::string &, Value);

	virtual Object *prototype();
	void set_prototype(Object *proto) { m_prototype = proto; }

	void set_native(const std::string &, const std::function<Value(Vm &, const std::vector<Value> &)> &);
	void set_native_property(const std::string &,
	                         const std::function<Value(Object *)> &,
	                         const std::function<void(Object *, Value)> &);

	bool has_own_property(const std::string &) const;
	bool has_own_property(const String &) const;
	virtual bool is_function() const { return false; }
	virtual bool is_native() const { return false; }
	virtual bool is_native_property() const { return false; }
	virtual bool is_bound_method() const { return false; }
	virtual bool is_bound_native_method() const { return false; }
	virtual bool is_closure() const { return false; }
	virtual bool is_array() const { return false; }
	virtual bool is_object() const { return true; }
	virtual bool is_date() const { return false; }

	Function *as_function();
	NativeFunction *as_native();
	NativeProperty *as_native_property();
	Closure *as_closure();
	Array *as_array();
	Date *as_date();

	const Array *as_array() const;

	// https://tc39.es/ecma262/#sec-ordinarytoprimitive
	Value ordinary_to_primitive(Vm &, const Value::Type &) const;

	std::unordered_map<std::string, Value> get_properties() const { return properties; }

	virtual std::string to_string() const;
	void print_prototype_chain();
protected:
	std::unordered_map<std::string, Value> properties;
	Object *m_prototype{nullptr};
};

class ObjectPrototype final : public Object
{
	friend class Heap;

public:
	ObjectPrototype(ObjectPrototype &other) = delete;
	void operator=(const ObjectPrototype &) = delete;
	Object *prototype() override { return nullptr; }

	static ObjectPrototype *the();

	std::string to_string() const override { return "ObjectPrototype{}"; }

private:
	ObjectPrototype();
	static ObjectPrototype *instance;
};
}
