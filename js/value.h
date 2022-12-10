#pragma once

#include <string>
#include <vector>

namespace js
{
class Function;

class Value
{
public:
	enum class Type
	{
		Array,
		Bool,
		Function,
		Null,
		Number,
		String,
		Undefined,
	};

	// construct undefined value
	Value()
	{
		m_type = Type::Undefined;
	}

	explicit Value(Type type) :
	    m_type(type)
	{ }

	explicit Value(std::nullptr_t p) :
		m_type(Type::Null)
	{ }

	explicit Value(double number) :
	    m_type(Type::Number),
	    number(number)
	{ }

	explicit Value(std::string *str) :
	    m_type(Type::String),
	    string(str)
	{ }

	explicit Value(std::vector<Value> *array) :
		m_type(Type::Array),
		array(array)
	{ }

	explicit Value(Function *function) :
		m_type(Type::Function),
		function(function)
	{ }

	inline Type type() const { return m_type; }

	inline bool is_array() const { return m_type == Type::Array; }
	inline bool is_bool() const { return m_type == Type::Bool; }
	inline bool is_function() const { return m_type == Type::Function; }
	inline bool is_null() const { return m_type == Type::Null; }
	inline bool is_number() const { return m_type == Type::Number; }
	inline bool is_string() const { return m_type == Type::String; }
	inline bool is_undefined() const { return m_type == Type::Undefined; }

	inline bool as_bool() const { return boolean; }
	inline double as_number() const { return number; }
	inline std::string *as_string() const { return string; }
	inline std::vector<Value> *as_array() const { return array; }
	inline Function *as_function() const { return function; }

	bool operator==(const Value &) const;

	bool is_falsy() const;
	std::string to_string() const;
	void store_at(int, Value);

private:
	Type m_type;
	union
	{
		bool boolean;
		double number;
		std::string *string;
		std::vector<Value> *array;
		Function *function;
	};
};
}
