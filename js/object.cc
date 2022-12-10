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
	if (!is_defined(key))
		return Value();

	return properties[key];
}

bool Object::is_defined(std::string const &key) const
{
	return properties.find(key) != properties.end();
}

std::string Object::to_string() const
{
	std::stringstream stream;
	stream << "{";

	for (auto it = properties.begin(); it != properties.end(); it++)
	{
		stream << " " << it->first;
		stream << ": ";

		if (it->second.as_object() == this)
			stream << "[Object object]";
		else
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
