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

private:
	std::unordered_map<std::string, Value> m_constants;
};
}
