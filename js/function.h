#pragma once

#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <string>
#include <vector>

#include "chunk.h"
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
	Function() = default;

	Function(FunctionType type) :
	    type(type)
	{ }

	Function(std::string const &name) :
	    name(name)
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
public:
	NativeFunction(const std::function<Value(Vm &vm, const std::vector<Value>)> &fn) :
	    fn(fn)
	{ }

	Value call(Vm &vm, const std::vector<Value> &argv) const { return fn(vm, argv); }
	bool is_native() const { return true; }

	std::string to_string() const { return "<native fn>"; }

private:
	std::function<Value(Vm &, std::vector<Value>)> fn;
};

struct BoundMethod final : public Object
{
	BoundMethod(Object *receiver, Closure *method) :
	    receiver(receiver),
	    method(method)
	{ }

	bool is_bound_method() const { return true; }

	Object *receiver;
	Closure *method;
};

class Closure final : public Object
{
public:
	Closure(Function *function) :
	    function(function)
	{
		upvalues.reserve(function->upvalue_count);
	}

	Function *function;
	std::vector<Upvalue *> upvalues;

	bool is_closure() const { return true; }
	std::string to_string() const { return function->to_string(); }
};
}
