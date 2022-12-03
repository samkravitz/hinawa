#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#include "../util/hinawa.h"
#include "value.h"

namespace js
{
class Chunk
{
public:
	void write(u8 byte, int line = 0);
	size_t add_constant(Value);
	void disassemble(const char *) const;
	size_t size() const;
	u32 allocate_register();
	inline size_t reg_count() const { return next_register; }

	std::vector<u8> code;
	std::vector<Value> constants;
	std::vector<int> lines;

	static constexpr u32 REG_INVALID = std::numeric_limits<u32>::max();
	static constexpr u32 REG_MAX = REG_INVALID - 1;

private:
	u32 next_register{ 1 };

	size_t disassemble_instruction(size_t) const;
	size_t simple_instruction(const char *, size_t) const;
	size_t constant_instruction(const char *, size_t) const;
	size_t byte_instruction(const char *, size_t) const;
	size_t jump_instruction(const char *, int, size_t) const;
	size_t array_instruction(const char *, size_t) const;
	size_t load_instruction(size_t) const;
	size_t binary_instruction(const char *, size_t) const;
};
}
