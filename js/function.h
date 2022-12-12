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
	Function()
	{
		anonymous = true;
	}

	Function(std::string const &name) :
	    name(name)
	{
		anonymous = false;
	}

	std::vector<Local> locals;
	size_t arity = 0;
	Chunk chunk;
	std::string name;
	int scope_depth = 0;
	bool anonymous;

	int local_count() const { return locals.size(); }

	std::string to_string() const
	{
		if (anonymous)
		{
			return "<anonymous fn>";
		}

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
