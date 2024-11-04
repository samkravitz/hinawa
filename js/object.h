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

struct Property
{
	enum Attributes : int
	{
		CONFIGURABLE = 0b001,
		ENUMERABLE = 0b010,
		WRITABLE = 0b100,
	};

	Property(Value value, int flags) :
	    value(value),
	    flags(flags)
	{ }

	Property() :
	    value(Value::js_undefined()),
	    flags(0)
	{ }

	Value value = {};
	int flags = 0;

	inline bool is_configurable() const { return flags & CONFIGURABLE; }
	inline bool is_enumerable() const { return flags & ENUMERABLE; }
	inline bool is_writable() const { return flags & WRITABLE; }

	constexpr static int default_attributes() { return CONFIGURABLE | ENUMERABLE | WRITABLE; }
};

class Object : public Cell
{
	friend class Heap;

public:
	virtual ~Object() { }

	Value get(const String &);
	Value get(const std::string &);
	void set(const String &, Value, int attributes = Property::default_attributes());
	void set(const std::string &, Value, int attributes = Property::default_attributes());

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

	std::unordered_map<std::string, Property> get_properties() const { return own_properties; }

	virtual std::string to_string() const;
	void print_prototype_chain();

protected:
	std::unordered_map<std::string, Property> own_properties;
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
