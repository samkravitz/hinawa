#include "value.h"

#include <cassert>
#include <cmath>
#include <sstream>

#include "error.h"
#include "function.h"
#include "object.h"
#include "vm.h"

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

Value Value::js_bigint(long num)
{
	Value value(Type::BigInt);
	value.number = (double) num;
	return value;
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

	if (type() == Type::Bool)
		return as_bool() == other.as_bool();

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
		case Type::BigInt:
			return fmt::format("{}n", (int) number);
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

std::expected<Value, Error> Value::to_primitive(Vm &vm, Type preferred_type) const
{
	//	1. If input is an Object, then
	if (is_object())
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

// https://tc39.es/ecma262/#sec-tonumber
std::expected<Value, Error> Value::to_number(Vm &vm) const
{
	// 1. If argument is a Number, return argument
	if (is_number())
		return *this;

	// 2. If argument is either a Symbol or a BigInt, throw a TypeError exception
	if (is_symbol() || is_bigint())
		return std::unexpected(*vm.heap().allocate<TypeError>());

	// 3. If argument is undefined, return NaN
	if (is_undefined())
		return js_nan();

	// 4. If argument is either null or false, return +0𝔽
	if (is_null() || (is_bool() && !as_bool()))
		return Value(0.0);

	// 5. If argument is true, return 1𝔽
	if (is_bool() && as_bool())
		return Value(1.0);

	// 6. If argument is a String, return StringToNumber(argument)
	if (is_string())
		return string_to_number();

	// 7. Assert: argument is an Object
	assert(is_object());

	// 8. Let prim be ? ToPrimitive(argument, NUMBER)
	auto prim = to_primitive(vm);
	if (!prim)
		return std::unexpected(prim.error());

	// 9. Assert: prim is not an Object
	assert(!prim->is_object());

	// 10. Return ? ToNumber(prim)
	return prim->to_number(vm);
}

std::expected<Value, Error> Value::to_numeric(Vm &vm) const
{
	// 1. Let prim be ? ToPrimitive(value, NUMBER)
	auto prim = to_primitive(vm);
	if (!prim)
		return std::unexpected(prim.error());

	// 2. If prim is a BigInt, return prim
	if (prim->is_bigint())
		return *prim;

	// 3. Return ? ToNumber(prim)
	return prim->to_number(vm);
}

const char *Value::type_of() const
{
	switch (type())
	{
		case Type::BigInt:
			return "bigint";
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

std::expected<Value, Error> apply_binary_operator(Vm &vm, const Value &lval, const Operator op, const Value &rval)
{
	// 1. If op is +, then
	if (op == Operator::Plus)
	{
		// a. Let lprim be ? ToPrimitive(lval)
		auto lprim = lval.to_primitive(vm);
		if (!lprim)
			return std::unexpected(lprim.error());

		// b. Let rprim be ? ToPrimitive(rval)
		auto rprim = rval.to_primitive(vm);
		if (!rprim)
			return std::unexpected(rprim.error());

		// c. If lprim is a String or rprim is a String, then
		if (lprim->is_string() || rprim->is_string())
		{
			// i. Let lstr be ? ToString(lprim)
			auto lstr = lprim->to_string();

			// ii. Let rstr be ? ToString(rprim)
			auto rstr = rprim->to_string();

			// iii. Return the string-concatenation of lstr and rstr
			return Value(vm.heap().allocate<String>(lstr + rstr));
		}

		// d. Set lval to lprim
		// e. Set rval to rprim
	}

	// 2. NOTE: At this point, it must be a numeric operation

	// 3. Let lnum be ? ToNumeric(lval)
	auto lnum = lval.to_numeric(vm);
	if (!lnum)
		return std::unexpected(lnum.error());

	// 4. Let rnum be ? ToNumeric(rval)
	auto rnum = rval.to_numeric(vm);
	if (!rnum)
		return std::unexpected(rnum.error());

	// 5. If Type(lnum) is not Type(rnum), throw a TypeError exception
	if (lnum->type() != rnum->type())
		std::unexpected(*vm.heap().allocate<TypeError>());

	// 6. If lnum is a BigInt, then
	if (lnum->is_bigint())
	{
		// a. If opText is **, return ? BigInt::exponentiate(lnum, rnum).
		// b. If opText is /, return ? BigInt::divide(lnum, rnum).
		// c. If opText is %, return ? BigInt::remainder(lnum, rnum).
		// d. If opText is >>>, return ? BigInt::unsignedRightShift(lnum, rnum).
	}

	// 7. Let operation be the abstract operation associated with opText and Type(lnum) in the following table
}
}
