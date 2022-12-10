#pragma once

#include <cstddef>
#include <vector>

#include "token.h"
#include "value.h"
#include "../util/hinawa.h"

namespace js
{
class Chunk
{
public:
	Chunk() = default;

	void write(u8, int);
	size_t add_constant(Value);
	void disassemble(const char *);
	size_t size();

	std::vector<u8> code;
	std::vector<Value> constants;
	std::vector<int> lines;

private:
	size_t disassemble_instruction(size_t);
	size_t simple_instruction(const char *, size_t);
	size_t constant_instruction(const char *, size_t);
	size_t byte_instruction(const char *, size_t);
	size_t jump_instruction(const char *, int, size_t);
	size_t array_instruction(const char *, size_t);
};
}
