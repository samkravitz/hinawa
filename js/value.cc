#include "value.h"

namespace js
{
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
}
