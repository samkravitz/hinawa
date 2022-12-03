#pragma once

#include "../util/hinawa.h"

namespace js
{
enum Opcode : u8
{
	OP_CONSTANT,
	OP_LOAD,
	OP_RETURN,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,

	OP_UNKNOWN,
};
}
