#pragma once

#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "document/document.h"
#include "error.h"
#include "function.h"
#include "operator.h"
#include "value.h"

namespace js
{
class String;
namespace bindings
{
class DocumentWrapper;
}

struct CallFrame
{
	CallFrame(Closure *closure, uint base) :
	    closure(closure),
	    base(base)
	{ }

	Closure *closure;
	uint ip{0};
	uint base{0};
	bool is_constructor{false};

	// pointer to the current this object
	Object *_this = nullptr;

	struct UnwindContext
	{
		unsigned ip;
		size_t sp;
	};

	std::vector<UnwindContext> unwind_contexts;
};

class Vm
{
	friend class Heap;

public:
	Vm();
	Vm(Document *);

	void interpret(const std::string &);
	Object *current_this() const;
	inline void set_global(Object *g) { m_global = g; }
	inline Object *global() { return m_global; }
	Heap &heap();

	Value call(const CallFrame &);
	Value call(Closure *);

	Document &document();
	void set_document_wrapper(bindings::DocumentWrapper *wrapper) { m_document_wrapper = wrapper; }

	void push(Value);
	Value pop();
	Value peek(uint offset = 0);
	std::string stack_trace() const;

	inline Error *error() const { return m_error; }
	inline bool has_error() const { return m_error != nullptr; }

	inline Value last_evaluated_expression() const { return m_last_evaluated_expression; }

private:
	// pointer to the global object
	Object *m_global = nullptr;

	Error *m_error = nullptr;

	Value m_last_evaluated_expression = {};

	std::string m_program_source = "";

	std::vector<Value> stack;
	std::vector<CallFrame> call_stack;

	void run_instruction(bool &);

	void binary_op(Operator);

	// returns call frame of the current executing function
	inline CallFrame &frame() { return call_stack.back(); }
	inline CallFrame frame() const { return call_stack.back(); }

	u8 read_byte();
	u16 read_short();
	Value read_constant();
	String &read_string();
	Upvalue *capture_upvalue(Value);
	bindings::DocumentWrapper *m_document_wrapper = nullptr;

	bool runtime_error(Error *, const std::string &);
	bool runtime_error(Value, const std::string &);

	void print_stack() const;
	void print_stack_trace() const;
	void print_nearby_lines() const;
};
}
