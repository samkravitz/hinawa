#pragma once

#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <string>
#include <vector>

#include "chunk.h"
#include "heap.h"
#include "object.h"
#include "string.hh"
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
	    Function(nullptr, FUNCTION)
	{ }

	Function(FunctionType type) :
	    Function(nullptr, type)
	{ }

	Function(const std::string &name) :
	    Function(heap().allocate_string(name), FUNCTION)
	{ }

	Function(String *name, FunctionType type) :
	    name(name),
	    type(type)
	{ }

	Function(const std::string &name, FunctionType type) :
	    Function(heap().allocate_string(name), type)
	{ }

	size_t arity = 0;
	Chunk chunk;
	String *name = nullptr;
	FunctionType type{FUNCTION};
	u8 upvalue_count{0};

	bool is_function() const { return true; }
	virtual bool is_native() const { return false; }

	std::string name_for_stack_trace() const
	{
		switch (type)
		{
			case ANONYMOUS:
				return "anonymous()";
			case SCRIPT:
				return "script";
			default:
				return fmt::format("{}()", name->string());
		}
	}

	virtual std::string to_string() const
	{
		switch (type)
		{
			case ANONYMOUS:
				return "<anonymous fn>";
			case SCRIPT:
				return "<script>";
			default:
				return fmt::format("<fn {}>", name->string());
		}
	}
};

class Vm;
class NativeFunction final : public Function
{
	friend class Heap;

public:
	static NativeFunction *create(const std::function<Value(Vm &vm, const std::vector<Value>)> &fn);

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
	friend class Heap;

public:
	static Closure *create(Function *function);

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

struct BoundNativeMethod final : public Object
{
	BoundNativeMethod(Object *receiver, NativeFunction *method) :
	    receiver(receiver),
	    method(method)
	{ }

	bool is_bound_native_method() const override { return true; }

	std::string to_string() const override
	{
		return fmt::format("{{ BoundNativeMethod this: {} method: {} }}", receiver->to_string(), method->to_string());
	}

	Object *receiver;
	NativeFunction *method;
};
}
