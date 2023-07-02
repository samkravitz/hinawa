#pragma once

#include <cstddef>
#include <vector>

#include "opcode.h"
#include "token.h"
#include "util/hinawa.h"
#include "value.h"

namespace js
{
class Chunk
{
public:
	Chunk() = default;

	void write(u8, int);
	size_t add_constant(Value);
	void disassemble(const char *);
	size_t disassemble_instruction(size_t);
	size_t size();

	std::vector<u8> code;
	std::vector<Value> constants;
	std::vector<int> lines;

private:
	size_t binary_instruction(Opcode, size_t);
	size_t simple_instruction(const char *, size_t);
	size_t constant_instruction(const char *, size_t);
	size_t byte_instruction(const char *, size_t);
	size_t jump_instruction(const char *, int, size_t);
	size_t new_object_instruction(const char *, size_t);
};
}
