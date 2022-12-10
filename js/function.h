#pragma once

#include <cstddef>
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

struct Function
{
	Function(std::string const &name);

	std::vector<Local> locals;
	size_t num_params;
	Chunk chunk;
	std::string name;
	bool native;
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
}
