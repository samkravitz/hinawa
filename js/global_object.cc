#include "global_object.h"

namespace js
{
Value GlobalObject::get(const String &primitive_string)
{
	const auto &key = primitive_string.string();

	if (own_properties.contains(key))
		return own_properties[key].value;

	if (m_constants.contains(key))
		return m_constants[key].value;

	return Value::js_undefined();
}

void GlobalObject::set_constant(const String &key, Value value)
{
	m_constants[key.string()] = Property(value, 0);
}

bool GlobalObject::has_own_property(const String &primitive_string) const
{
	const auto &key = primitive_string.string();

	if (own_properties.contains(key))
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