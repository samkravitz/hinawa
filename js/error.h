#pragma once

#include <fmt/format.h>
#include <string>

#include "object.h"

namespace js
{
class Error : public Object
{
public:
	virtual const char *name() const { return "Error"; }
};

class ReferenceError final : public Error
{
public:
	ReferenceError() :
	    ReferenceError("")
	{ }

	ReferenceError(const std::string &identifier) :
	    m_identifier(identifier)
	{ }

	std::string to_string() const override { return fmt::format("{}: {} is not defined.", name(), m_identifier); }

	virtual const char *name() const override { return "ReferenceError"; }

private:
	// undefined identifier that was used
	std::string m_identifier = "";
};

class TypeError final : public Error
{
public:
	virtual const char *name() const override { return "TypeError"; }
};
}