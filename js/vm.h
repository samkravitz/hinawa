#pragma once

#include <list>
#include <vector>

#include "error.h"
#include "function.h"
#include "global_object.h"
#include "operator.h"
#include "value.h"

#ifdef JS_BUILD_BINDINGS
	#include "document/document.h"
#endif

namespace js
{
class String;
namespace bindings
{
#ifdef JS_BUILD_BINDINGS
class DocumentWrapper;
#endif
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

#ifdef JS_BUILD_BINDINGS
	Vm(Document *);
#endif

	void interpret(const std::string &);
	Object *current_this() const;
	inline void set_global(GlobalObject *g) { m_global = g; }
	inline GlobalObject &global() { return *m_global; }
	Heap &heap();

	Value call(const CallFrame &);
	Value call(Closure *);

#ifdef JS_BUILD_BINDINGS
	Document &document();
	void set_document_wrapper(bindings::DocumentWrapper *wrapper) { m_document_wrapper = wrapper; }
#endif

	void push(Value);
	Value pop();
	Value peek(uint offset = 0);
	std::string stack_trace() const;

	inline Error *error() const { return m_error; }
	inline bool has_error() const { return m_error != nullptr; }

	inline Value last_evaluated_expression() const { return m_last_evaluated_expression; }

private:
	// pointer to the global object
	GlobalObject *m_global = nullptr;

	Error *m_error = nullptr;

	Value m_last_evaluated_expression = {};

	std::string m_program_source = "";

	std::vector<Value> stack;
	std::vector<CallFrame> call_stack;
	std::list<Upvalue *> open_upvalues;

	void run_instruction(bool &);

	void binary_op(Operator);

	// returns call frame of the current executing function
	inline CallFrame &frame() { return call_stack.back(); }
	inline CallFrame frame() const { return call_stack.back(); }

	u8 read_byte();
	u16 read_short();
	Value read_constant();
	String &read_string();
	Upvalue *capture_upvalue(u8);
	void close_upvalues(u8);

#ifdef JS_BUILD_BINDINGS
	bindings::DocumentWrapper *m_document_wrapper = nullptr;
#endif

	bool runtime_error(Error *, const std::string &);
	bool runtime_error(Value, const std::string &);

	void print_stack() const;
	void print_stack_trace() const;
	void print_nearby_lines() const;
};
}
