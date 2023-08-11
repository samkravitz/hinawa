#include "value.h"

#include <cassert>
#include <cmath>
#include <sstream>

#include "function.h"
#include "object.h"

namespace js
{

Value Value::js_null()
{
	return Value(Type::Null);
}

Value Value::js_undefined()
{
	return {};
}

Value Value::js_nan()
{
	return Value(NAN);
}

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
		const auto &str1 = as_string();
		const auto &str2 = other.as_string();
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

	return true;
}

Value Value::operator+(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	auto a = as_number();
	auto b = other.as_number();
	return Value(a + b);
}

Value Value::operator-(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	auto a = as_number();
	auto b = other.as_number();
	return Value(a - b);
}

Value Value::operator*(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	auto a = as_number();
	auto b = other.as_number();
	return Value(a * b);
}

Value Value::operator/(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	auto a = as_number();
	auto b = other.as_number();
	return Value(a / b);
}

Value Value::operator%(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	int a = (int) as_number();
	int b = (int) other.as_number();
	return Value(static_cast<double>(a % b));
}

Value Value::operator<(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return Value(false);

	auto a = as_number();
	auto b = other.as_number();
	return Value(a < b);
}

Value Value::operator>(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return Value(false);

	auto a = as_number();
	auto b = other.as_number();
	return Value(a > b);
}

Value Value::operator&(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	int a = (int) as_number();
	int b = (int) other.as_number();
	return Value(static_cast<double>(a & b));
}

Value Value::operator&&(const Value &other) const
{
	if (is_falsy())
		return *this;

	return other;
}

Value Value::operator|(const Value &other) const
{
	if (!is_number() || !other.is_number())
		return js_nan();

	int a = (int) as_number();
	int b = (int) other.as_number();
	return Value(static_cast<double>(a | b));
}

Value Value::operator||(const Value &other) const
{
	if (is_truthy())
		return *this;

	return other;
}

std::string Value::to_string() const
{
	switch (type())
	{
		case Type::Bool:
			return as_bool() ? "true" : "false";
		case Type::Null:
			return "null";
		case Type::Number:
		{
			double num = as_number();
			if (std::isnan(num))
				return "NaN";
			if (num == std::trunc(num))
				return std::to_string((int) num);
			return std::to_string(num);
		}
		case Type::Object:
			return as_object()->to_string();
		case Type::Undefined:
			return "undefined";
		case Type::String:
			return as_string().string();
		default:
			assert(!"Unknown value type!");
			return "";
	}
}

bool Value::is_nan() const
{
	if (!is_number())
		return false;

	return std::isnan(as_number());
}

bool Value::is_falsy() const
{
	if (is_bool())
		return !as_bool();

	if (is_null() || is_undefined())
		return true;

	return false;
}

Value Value::to_primitive(Type preferred_type) const
{
	//	1. If input is an Object, then
	if (is_object)
	{
		//	a. Let exoticToPrim be ? GetMethod(input, @@toPrimitive).
		//	b. If exoticToPrim is not undefined, then
		// 		i. If preferredType is not present, then
		//			1. Let hint be "default".
		// 		ii. Else if preferredType is string, then
		//			1. Let hint be "string".
		// 		iii. Else,
		//			1. Assert: preferredType is number.
		//			2. Let hint be "number".
		// 		iv. Let result be ? Call(exoticToPrim, input, « hint »).
		// 		v. If result is not an Object, return result.
		// 		vi. Throw a TypeError exception.
		//	c. If preferredType is not present, let preferredType be number.

		//	d. Return ? OrdinaryToPrimitive(input, preferredType).
		return as_object()->ordinary_to_primitive(preferred_type);
	}

	// 2. Return input.
	return *this;
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
