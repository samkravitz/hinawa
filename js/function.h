#pragma once

#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <string>
#include <vector>

#include "chunk.h"
#include "heap.h"
#include "object.h"
#include "token.h"

namespace js
{
struct Local
{
	Local(const std::string &name, int depth) :
	    name{name},
	    depth{depth}
	{ }

	std::string name;
	int depth;
};

enum FunctionType
{
	SCRIPT,
	FUNCTION,
	ANONYMOUS,
};

class Upvalue final : public Object
{
public:
	Upvalue(Value *location) :
	    location(location)
	{ }

	Value *location;
};

class Function : public Object
{
public:
	Function() :
	    Function("", FUNCTION)
	{ }

	Function(FunctionType type) :
	    Function("", type)
	{ }

	Function(std::string const &name) :
	    Function(name, FUNCTION)
	{ }

	Function(std::string const &name, FunctionType type) :
	    name(name),
	    type(type)
	{ }

	size_t arity = 0;
	Chunk chunk;
	std::string name;
	FunctionType type{FUNCTION};
	u8 upvalue_count{0};

	bool is_function() const { return true; }
	virtual bool is_native() const { return false; }

	virtual std::string to_string() const
	{
		switch (type)
		{
			case ANONYMOUS:
				return "<anonymous fn>";
			case SCRIPT:
				return "<script>";
			default:
				return fmt::format("<fn {}>", name);
		}
	}
};

class Vm;
class NativeFunction final : public Function
{
	friend class Heap;

public:
	static NativeFunction *create(const std::function<Value(Vm &vm, const std::vector<Value>)> &fn)
	{
		auto *native = heap().allocate<NativeFunction>(fn);

		/**
		* functions have a property "prototype", that is an object
		* with the property "constructor", which holds a reference
		* to the function. When a new instance is created from the
		* function with the 'new' keyword, the created object's
		* prototype is the beforementioned object.
		*/
		auto *object = heap().allocate<Object>();
		object->set("constructor", Value(native));
		native->set("prototype", Value(object));

		return native;
	}

	Value call(Vm &vm, const std::vector<Value> &argv) const { return fn(vm, argv); }
	bool is_native() const { return true; }

	std::string to_string() const { return "<native fn>"; }

private:
	NativeFunction(const std::function<Value(Vm &vm, const std::vector<Value>)> &fn) :
	    fn(fn)
	{ }

	std::function<Value(Vm &, std::vector<Value>)> fn;
};

class Closure final : public Object
{
public:
	static Closure *create(Function *function)
	{
		auto *closure = new Closure(function);

		/**
		* functions have a property "prototype", that is an object
		* with the property "constructor", which holds a reference
		* to the function. When a new instance is created from the
		* function with the 'new' keyword, the created object's
		* prototype is the beforementioned object.
		*/
		auto *object = heap().allocate<Object>();
		object->set("constructor", Value(closure));
		closure->set("prototype", Value(object));

		return closure;
	}

	Function *function;
	std::vector<Upvalue *> upvalues;

	bool is_closure() const { return true; }
	std::string to_string() const { return function->to_string(); }

private:
	Closure(Function *function) :
	    function(function)
	{
		upvalues.reserve(function->upvalue_count);
	}
};

struct BoundMethod final : public Object
{
	BoundMethod(Object *receiver, Closure *method) :
	    receiver(receiver),
	    method(method)
	{ }

	bool is_bound_method() const override { return true; }

	std::string to_string() const override
	{
		return fmt::format("{{ BoundMethod this: {} method: {} }}", receiver->to_string(), method->to_string());
	}

	Object *receiver;
	Closure *method;
};
}
