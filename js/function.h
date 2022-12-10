#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "chunk.h"
#include "token.h"

namespace js
{
struct Local
{
	Local(Token name, int depth) :
	    name(name),
	    depth(depth)
	{ }

	Token name;
	int depth;
};

class Function
{
public:
	Function(std::string const &name) :
		name(name)
	{
		num_params = 0;
	}

	std::vector<Local> locals;
	size_t num_params;
	Chunk chunk;
	std::string name;
	int scope_depth = 0;

	int local_count() const { return locals.size(); }

	std::string to_string() const
	{
		std::string res = "<fn ";
		res += name;
		res += ">";
		return res;
	}
};

class NativeFunction
{
public:
	NativeFunction(const std::function<Value(std::vector<Value>)> &fn) :
	    fn(fn)
	{ }

	Value call(const std::vector<Value> &argv) const
	{
		return fn(argv);
	}

private:
	std::function<Value(std::vector<Value>)> fn;
};
}
