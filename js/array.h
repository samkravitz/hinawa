#pragma once

#include <sstream>
#include <vector>

#include "object.h"

namespace js
{
class Array final : public Object, public std::vector<Value>
{
public:
	Array(std::vector<Value> array)
	{
		for (auto element : array)
			push_back(element);
	}

	bool is_array() const { return true; }
	std::string to_string() const
	{
		std::stringstream stream;
		std::vector<Value> vec = *this;
		stream << "[";
		for (unsigned i = 0; i < vec.size(); i++)
		{
			stream << vec[i].to_string();
			if (i != vec.size() - 1)
				stream << ", ";
		}
		stream << "]";
		return stream.str();
	}
};
}
