#include "value.h"

namespace js
{
bool Value::operator==(const Value &other) const
{
	if (type() != other.type())
		return false;
	
	if (type() == Type::String)
	{
		std::string str1 = *as_string();
		std::string str2 = *other.as_string();
		return str1 == str2;
	}

	if (type() == Type::Number)
	{
		double a = as_number();
		double b = other.as_number();
		return a == b;
	}

	return false;
}

std::string Value::to_string() const
{
	switch (type())
	{
		case Type::Bool: return as_bool() ? "true" : "false";
		case Type::Null: return "null";
		case Type::Number: return std::to_string(as_number());
		case Type::Undefined: return "undefined";
		case Type::String: return *as_string();
	}
}

bool Value::is_falsy() const
{
	if (is_bool())
		return !as_bool();
	
	if (is_null() || is_undefined())
		return true;
	
	return false;
}

void Value::store_at(int index, Value v)
{
	auto array = *as_array();
	array[index] = v;
	//value = array;
}
}
