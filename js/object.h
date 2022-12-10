#pragma once

#include <string>
#include <unordered_map>

#include "value.h"

namespace js
{
class Object
{
public:
	Value get(std::string const &);
	void set(std::string, Value);

	bool is_defined(std::string const &) const;

	std::string to_string() const;

private:
	std::unordered_map<std::string, Value> properties;
};
}
