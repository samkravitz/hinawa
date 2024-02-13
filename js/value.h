#pragma once

#include <expected>
#include <string>
#include <vector>

#include "operator.h"
#include "string.hh"

namespace js
{
class Function;
class NativeFunction;
class Object;
class Error;
class Vm;

class Value
{
public:
	enum class Type
	{
		BigInt,
		Bool,
		Null,
		Number,
		Object,
		String,
		Symbol,
		Undefined,
	};

	// construct undefined value
	Value() { m_type = Type::Undefined; }

	explicit Value(Type type) :
	    m_type(type)
	{ }

	explicit Value(std::nullptr_t p) :
	    m_type(Type::Null)
	{ }

	explicit Value(bool boolean) :
	    m_type(Type::Bool),
	    boolean(boolean)
	{ }

	explicit Value(double number) :
	    m_type(Type::Number),
	    number(number)
	{ }

	explicit Value(String *str) :
	    m_type(Type::String),
	    string(str)
	{ }

	explicit Value(Object *object) :
	    m_type(Type::Object),
	    object(object)
	{ }

	inline Type type() const { return m_type; }

	// equality operator, == in JS
	bool eq(const Value &) const;

	// strict equality operator, === in JS
	bool strict_eq(const Value &) const;

	// in C++, our == operator will be equivalent to strict equals
	bool operator==(const Value &other) const { return this->strict_eq(other); }

	static Value js_null();
	static Value js_undefined();
	static Value js_nan();
	static Value js_bigint(long);
	static Value js_negative_zero();
	static Value js_zero();
	bool is_negative_zero() const;
	bool is_zero() const;

	inline bool is_bigint() const { return m_type == Type::BigInt; }
	inline bool is_bool() const { return m_type == Type::Bool; }
	inline bool is_null() const { return m_type == Type::Null; }
	inline bool is_number() const { return m_type == Type::Number; }
	inline bool is_string() const { return m_type == Type::String; }
	inline bool is_object() const { return m_type == Type::Object; }
	inline bool is_symbol() const { return m_type == Type::Symbol; }
	inline bool is_undefined() const { return m_type == Type::Undefined; }

	Value operator+(const Value &) const;
	Value operator-(const Value &) const;
	Value operator*(const Value &) const;
	Value operator/(const Value &) const;
	Value operator%(const Value &) const;
	Value operator<(const Value &) const;
	Value operator>(const Value &) const;
	Value operator&(const Value &) const;
	Value operator&&(const Value &) const;
	Value operator|(const Value &) const;
	Value operator||(const Value &) const;

	inline bool as_bool() const { return boolean; }
	inline Object *as_object() const { return object; }
	inline double as_number() const { return number; }
	inline String &as_string() const { return *string; }

	bool is_falsy() const;
	inline bool is_truthy() const { return !is_falsy(); }
	bool is_nan() const;
	const char *type_of() const;
	std::string to_string() const;

	// https://tc39.es/ecma262/#sec-toprimitive
	std::expected<Value, Error> to_primitive(Vm &, Type preferred_type = Type::Number) const;

	// https://tc39.es/ecma262/#sec-tonumber
	std::expected<Value, Error> to_number(Vm &) const;

	// https://tc39.es/ecma262/#sec-tonumeric
	std::expected<Value, Error> to_numeric(Vm &) const;

	// https://tc39.es/ecma262/#sec-stringtonumber
	Value string_to_number() const { return Value(0.0); }

private:
	Type m_type;
	union
	{
		bool boolean;
		Object *object;
		double number;
		String *string;
	};
};

// https://tc39.es/ecma262/#sec-applystringornumericbinaryoperator
std::expected<Value, Error> apply_binary_operator(Vm &, const Value &, const Operator, const Value &);
}
