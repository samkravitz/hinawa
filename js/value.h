#pragma once

#include <string>

namespace js
{
class Value
{
public:
	enum class Type
	{
		Boolean,
		Null,
		Undefined,
		Number,
		String,
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

	inline Type type() const { return m_type; }

	inline bool as_boolean() const { return boolean; }
	inline double as_number() const { return number; }
	inline std::string *as_string() const { return string; }

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
