#include "value.h"

#include <cassert>
#include <cmath>
#include <sstream>

#include "object.h"
#include "function.h"

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

	if (type() == Type::Object)
	{
		Object *a = as_object();
		Object *b = other.as_object();
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
		case Type::Number:
		{
			double num = as_number();
			if (num == std::trunc(num))
				return std::to_string((int) num);
			return std::to_string(num);
		}
		case Type::Object: return as_object()->to_string();
		case Type::Undefined: return "undefined";
		case Type::String: return *as_string();
		default:
			assert(!"Unknown value type!");
			return "";
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
}
