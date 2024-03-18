#include "global_object.h"

namespace js
{
Value GlobalObject::get(const String &primitive_string)
{
	const auto &key = primitive_string.string();

	if (properties.contains(key))
		return properties[key];

	if (m_constants.contains(key))
		return m_constants[key];

	return Value::js_undefined();
}

void GlobalObject::set_constant(const String &key, Value value)
{
	m_constants[key.string()] = value;
}

bool GlobalObject::has_own_property(const String &primitive_string) const
{
	const auto &key = primitive_string.string();

	if (properties.contains(key))
		return true;

	if (m_constants.contains(key))
		return true;

	return false;
}

bool GlobalObject::has_constant(const String &primitive_string) const
{
	return m_constants.contains(primitive_string.string());
}
}