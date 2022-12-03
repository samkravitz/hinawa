#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#include "value.h"
#include "../util/hinawa.h"

namespace js
{
static constexpr u32 REG_MAX = std::numeric_limits<u32>::max();

class Chunk
{
public:
	void write(u8 byte, int line = 0);
	size_t add_constant(Value);
	void disassemble(const char *) const;
	size_t size() const;
	u32 allocate_register();

private:
	std::vector<u8> m_code;
	std::vector<Value> m_constants;
	std::vector<int> m_lines;
	u32 m_next_register { 1 };

	size_t disassemble_instruction(size_t) const;
	size_t simple_instruction(const char *, size_t) const;
	size_t constant_instruction(const char *, size_t) const;
	size_t byte_instruction(const char *, size_t) const;
	size_t jump_instruction(const char *, int, size_t) const;
	size_t array_instruction(const char *, size_t) const;
};
}
