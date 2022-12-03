#pragma once

#include <cstddef>
#include <vector>

#include "value.h"
#include "../util/hinawa.h"

namespace js
{
class Chunk
{
public:
	void write(u8 byte, int line = 0);
	size_t add_constant(Value);
	void disassemble(const char *) const;
	size_t size() const;

private:
	std::vector<u8> m_code;
	std::vector<Value> m_constants;
	std::vector<int> m_lines;

	size_t disassemble_instruction(size_t) const;
	size_t simple_instruction(const char *, size_t) const;
	size_t constant_instruction(const char *, size_t) const;
	size_t byte_instruction(const char *, size_t) const;
	size_t jump_instruction(const char *, int, size_t) const;
	size_t array_instruction(const char *, size_t) const;
};
}
