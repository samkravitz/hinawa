#include "object.h"

#include <sstream>

namespace js
{
void Object::set(std::string key, Value value)
{
	properties[key] = value;
}

Value Object::get(std::string const &key)
{
	if (properties.find(key) == properties.end())
		return Value();
	
	return properties[key];
}

std::string Object::to_string() const
{
	std::stringstream stream;
	stream << "{";

	for (auto it = properties.begin(); it != properties.end(); it++)
	{
		stream << " " << it->first;
		stream << ": ";
		stream << it->second.to_string();
		if (std::next(it) != properties.end())
			stream << ", ";
		else
			stream << " ";
	}

	stream << "}";
	return stream.str();
}
}
