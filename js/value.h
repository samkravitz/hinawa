#pragma once

#include <string>

namespace js
{
class Value
{
public:
	enum class Type
	{
		Bool,
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

	explicit Value(double number) :
	    m_type(Type::Number),
	    number(number)
	{ }

	explicit Value(std::string *str) :
	    m_type(Type::String),
	    string(str)
	{ }

	inline Type type() const { return m_type; }

	inline bool as_bool() const { return boolean; }
	inline double as_number() const { return number; }
	inline std::string *as_string() const { return string; }

	std::string to_string() const;

private:
	Type m_type;
	union
	{
		bool boolean;
		double number;
		std::string *string;
	};
};
}
