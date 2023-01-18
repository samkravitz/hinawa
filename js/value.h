#pragma once

#include <string>
#include <vector>

namespace js
{
class Function;
class NativeFunction;
class Object;

class Value
{
public:
	enum class Type
	{
		Array,
		Bool,
		Function,
		Native,
		Null,
		Number,
		Object,
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

	explicit Value(bool boolean) :
	    m_type(Type::Bool),
	    boolean(boolean)
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

	explicit Value(Object *object) :
		m_type(Type::Object),
		object(object)
	{ }

	inline Type type() const { return m_type; }

	inline bool is_array() const { return m_type == Type::Array; }
	inline bool is_bool() const { return m_type == Type::Bool; }
	bool is_function() const;
	bool is_native() const;
	inline bool is_null() const { return m_type == Type::Null; }
	inline bool is_number() const { return m_type == Type::Number; }
	inline bool is_string() const { return m_type == Type::String; }
	inline bool is_object() const { return m_type == Type::Object; }
	inline bool is_undefined() const { return m_type == Type::Undefined; }

	inline std::vector<Value> *as_array() const { return array; }
	inline bool as_bool() const { return boolean; }
	Function *as_function() const;
	NativeFunction *as_native() const;
	inline Object *as_object() const { return object; }
	inline double as_number() const { return number; }
	inline std::string *as_string() const { return string; }

	bool operator==(const Value &) const;

	bool is_falsy() const;
	std::string to_string() const;

private:
	Type m_type;
	union
	{
		std::vector<Value> *array;
		bool boolean;
		Object *object;
		double number;
		std::string *string;
	};
};
}
