#pragma once

#include "util/hinawa.h"

namespace js
{
enum Opcode : u8
{
	OP_RETURN,
	OP_CONSTANT,
	OP_NEGATE,
	OP_INCREMENT,
	OP_DECREMENT,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_MOD,
	OP_NULL,
	OP_UNDEFINED,
	OP_TRUE,
	OP_FALSE,
	OP_NOT,
	OP_EQUAL,
	OP_STRICT_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_LOGICAL_AND,
	OP_LOGICAL_OR,
	OP_BITWISE_AND,
	OP_BITWISE_OR,
	OP_POP,
	OP_DEFINE_GLOBAL,
	OP_DEFINE_CONSTANT,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_GET_LOCAL,
	OP_SET_LOCAL,
	OP_JUMP_IF_FALSE,
	OP_JUMP,
	OP_LOOP,
	OP_CALL,
	OP_NEW_ARRAY,
	OP_GET_SUBSCRIPT,
	OP_SET_SUBSCRIPT,
	OP_CLASS,
	OP_GET_PROPERTY,
	OP_SET_PROPERTY,
	OP_NEW_OBJECT,
	OP_PUSH_EXCEPTION,
	OP_POP_EXCEPTION,
	OP_THROW,
	OP_CLOSURE,
	OP_GET_UPVALUE,
	OP_SET_UPVALUE,
	OP_CLOSE_UPVALUE,
	OP_CALL_CONSTRUCTOR,
	OP_INSTANCEOF,
	OP_TYPEOF,
	OP_DEBUGGER,
	OP_NOOP,
	OP_POP_N,
};
}
