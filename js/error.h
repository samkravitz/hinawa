#pragma once

#include <fmt/format.h>
#include <string>

#include "object.h"

namespace js
{
class Vm;
class Value;

class Error : public Object
{
public:
	Error() = default;
	Error(Vm &);
	Error(Vm &, const std::string &);

	std::string stack_trace() const { return m_stack_trace; }
	std::string message() const { return m_message; }

	virtual const char *name() const { return "Error"; }

private:
	std::string m_stack_trace = "";
	std::string m_message = "";
};

class ReferenceError final : public Error
{
public:
	ReferenceError(Vm &, const std::string &);

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