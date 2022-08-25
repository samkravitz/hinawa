#pragma once

#include <sstream>
#include <string>

namespace layout
{
struct Edges
{
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;

	std::string to_string() const
	{
		std::stringstream ss;
		ss << "{ ";
		ss << "l: " << left;
		ss << " r: " << right;
		ss << " t: " << top;
		ss << " b: " << bottom;
		ss << " }";

		return ss.str();
	}
};
}
