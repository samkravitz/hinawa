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

	return false;
}

bool Value::is_function() const
{
	return is_object() && as_object()->is_function();
}

bool Value::is_native() const
{
	return is_object() && as_object()->is_native();
}

Function *Value::as_function() const
{
	assert(is_function());
	return static_cast<Function*>(object);
}

NativeFunction *Value::as_native() const
{
	assert(is_native());
	return static_cast<NativeFunction*>(object);
}

std::string Value::to_string() const
{
	switch (type())
	{
		case Type::Array:
		{
			std::stringstream stream;
			std::vector<Value> vec = *as_array();
			stream <<  "[";
			for (uint i = 0; i < vec.size(); i++)
			{
				stream << vec[i].to_string();
				if (i != vec.size() - 1)
					stream << ", ";
			}
			stream << "]";
			return stream.str();
		}			
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
