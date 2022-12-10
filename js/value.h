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

	explicit Value(NativeFunction *native) :
		m_type(Type::Native),
		native(native)
	{ }

	explicit Value(Object *object) :
		m_type(Type::Object),
		object(object)
	{ }

	inline Type type() const { return m_type; }

	inline bool is_array() const { return m_type == Type::Array; }
	inline bool is_bool() const { return m_type == Type::Bool; }
	inline bool is_function() const { return m_type == Type::Function; }
	inline bool is_native() const { return m_type == Type::Native; }
	inline bool is_null() const { return m_type == Type::Null; }
	inline bool is_number() const { return m_type == Type::Number; }
	inline bool is_string() const { return m_type == Type::String; }
	inline bool is_object() const { return m_type == Type::Object; }
	inline bool is_undefined() const { return m_type == Type::Undefined; }

	inline std::vector<Value> *as_array() const { return array; }
	inline bool as_bool() const { return boolean; }
	inline Function *as_function() const { return function; }
	inline NativeFunction *as_native() const { return native; }
	inline Object *as_object() const { return object; }
	inline double as_number() const { return number; }
	inline std::string *as_string() const { return string; }

	bool operator==(const Value &) const;

	bool is_falsy() const;
	std::string to_string() const;
	void store_at(int, Value);

private:
	Type m_type;
	union
	{
		std::vector<Value> *array;
		bool boolean;
		Function *function;
		NativeFunction *native;
		Object *object;
		double number;
		std::string *string;
	};
};
}
