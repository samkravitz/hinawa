#include "error.h"

#include "vm.h"

namespace js
{
Error::Error(Vm &vm, const std::string &message) :
    m_stack_trace(vm.stack_trace()),
    m_message(message)
{
	auto stack_trace_value = Value(new String(m_stack_trace));
	auto message_value = Value(new String(m_message));
	set("stack", stack_trace_value);
	set("message", message_value);
}

Error::Error(Vm &vm) :
    Error(vm, "")
{ }

ReferenceError::ReferenceError(Vm &vm, const std::string &identifier) :
    Error(vm),
    m_identifier(identifier)
{ }
}