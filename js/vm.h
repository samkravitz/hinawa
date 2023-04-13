#pragma once

#include <stack>
#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "function.h"
#include "value.h"

namespace js
{
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
	CallFrame(Function f, uint base) :
	    function(f),
	    base(base)
	{ }

	Function function;
	uint ip{0};
	uint base{0};
	
	struct CatchEnv
	{
		unsigned ip;
		size_t sp;
	};

	std::vector<CatchEnv> catchv;
};

class Vm
{
public:
	Vm(bool headless = false);
	bool run(Function);
	Object *current_this() const { return _this; }

private:
	// pointer to the global object
	Object *global = nullptr;

	// pointer to the current this object
	Object *_this = nullptr;

	std::vector<Value> stack;
	std::stack<CallFrame> frames;

	void push(Value);
	Value pop();
	Value peek(uint offset = 0);

	void binary_op(Operator);

	u8 read_byte();
	u16 read_short();
	Value read_constant();
	std::string read_string();

	bool runtime_error(std::string const &);
};
}
