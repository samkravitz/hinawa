#pragma once

#include <vector>

#include "chunk.h"
#include "value.h"

namespace js
{
class Vm
{
public:
	Value run(Chunk const &);

private:
	std::vector<Value> reg;
	uint ip{ 0 };
	Chunk chunk;

	u8 read_byte();
	u16 read_short();
	Value read_constant();
};
}
