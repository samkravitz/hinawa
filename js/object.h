#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "value.h"

namespace js
{
class Vm;
class Function;
class NativeFunction;
class Closure;
class Array;

class Object
{
public:
	virtual ~Object() { }

	Value get(std::string const &);
	void set(std::string, Value);
	void set_native(const std::string &, const std::function<Value(Vm &, const std::vector<Value>&)> &);

	bool has_own_property(std::string const &) const;
	virtual bool is_function() const { return false; }
	virtual bool is_native() const { return false; }
	virtual bool is_bound_method() const { return false; }
	virtual bool is_closure() const { return false; }
	virtual bool is_array() const { return false; }

	Function *as_function();
	NativeFunction *as_native();
	Closure *as_closure();
	Array *as_array();

	virtual std::string to_string() const;

protected:
	std::unordered_map<std::string, Value> properties;
	virtual Object *prototype();
};

class ObjectPrototype final : public Object
{
public:
	ObjectPrototype(ObjectPrototype &other) = delete;
	void operator=(const ObjectPrototype &) = delete;
	Object *prototype() override { return nullptr; }

	static ObjectPrototype *the();

private:
	ObjectPrototype();
	static ObjectPrototype *instance;
};
}
