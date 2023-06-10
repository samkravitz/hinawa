#include "value.h"

#include <cassert>
#include <cmath>
#include <sstream>

#include "object.h"
#include "function.h"

namespace js
{
bool Value::eq(const Value &other) const
{
	// TODO - for now, === and == are equivalent
	return strict_eq(other);
}

bool Value::strict_eq(const Value &other) const
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

const char *Value::type_of() const
{
	switch (type())
	{
		case Type::Bool:
			return "boolean";
		case Type::Number:
			return "number";
		case Type::Null:
			return "object";
		case Type::Object:
		{
			auto *obj = as_object();
			if (obj->is_closure() || obj->is_native() || obj->is_bound_method())
				return "function";

			return "object";
		}
		case Type::Undefined:
			return "undefined";
		case Type::String:
			return "string";
		default:
			assert(!"Unknown value type!");
			return "";
	}
}
}
