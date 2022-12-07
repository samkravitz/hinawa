#pragma once

#include <vector>

#include "chunk.h"
#include "value.h"

namespace js
{
class Vm
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

public:
	Value run(Chunk const &);

private:
	std::vector<Value> reg;
	uint ip{ 0 };
	Chunk chunk;

	void binary_op(Operator);
	u8 read_byte();
	u16 read_short();
	Value read_constant();
};
}
