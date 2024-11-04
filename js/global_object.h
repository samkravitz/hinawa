#pragma once

#include <unordered_map>

#include "object.h"

namespace js
{
class GlobalObject final : public Object
{
public:
	Value get(const String &);
	void set_constant(const String &, Value);
	bool has_own_property(const String &) const;
	bool has_constant(const String &) const;

private:
	std::unordered_map<std::string, Property> m_constants;
};
}
