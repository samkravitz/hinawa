#pragma once

#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "document/document.h"
#include "function.h"
#include "value.h"

namespace js
{
class String;
namespace bindings
{
class DocumentWrapper;
}

enum class Operator
{
	Plus,
	Minus,
	Star,
	Slash,
	Mod,
	LessThan,
	GreaterThan,
	Amp,
	AmpAmp,
	Pipe,
	PipePipe,
};

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

	struct CatchEnv
	{
		unsigned ip;
		size_t sp;
	};

	std::vector<CatchEnv> catchv;
};

class Vm
{
	friend class Heap;

public:
	Vm();
	Vm(Document *);

	bool run(Function &);
	Object *current_this() const { return _this; }
	void set_global(Object *g) { global = g; }
	void call(Closure *);

	Document &document();
	void set_document_wrapper(bindings::DocumentWrapper *wrapper) { m_document_wrapper = wrapper; }

	void push(Value);
	Value pop();
	Value peek(uint offset = 0);

	bool has_error() const { return m_has_error; }

private:
	// pointer to the global object
	Object *global = nullptr;

	// pointer to the current this object
	Object *_this = nullptr;

	bool m_has_error{false};

	std::vector<Value> stack;
	std::vector<CallFrame> frames;

	bool run_instruction(bool in_call);

	void binary_op(Operator);

	u8 read_byte();
	u16 read_short();
	Value read_constant();
	String &read_string();
	Upvalue *capture_upvalue(Value *);
	bindings::DocumentWrapper *m_document_wrapper = nullptr;

	void print_stack() const;
	bool runtime_error(std::string const &);
};
}
