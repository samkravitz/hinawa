#pragma once

#include <string>

#include "cell.h"

namespace js
{
class PrimitiveString : public Cell
{
public:
	explicit PrimitiveString(std::string str) { m_string = new std::string(str); }

	~PrimitiveString() { delete m_string; }

	std::string &string() { return *m_string; }
	const std::string &string() const { return *m_string; }

	std::string to_string() const override { return string(); }

	bool operator==(const PrimitiveString &other) const { return string() == other.string(); }

private:
	std::string *m_string = nullptr;
};
}
