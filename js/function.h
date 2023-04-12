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
	BoundMethod(Object *receiver, Function *method) :
	    receiver(receiver),
	    method(method)
	{ }

	bool is_bound_method() const { return true; }

	Object *receiver;
	Function *method;
};
}
