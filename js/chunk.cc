#include "chunk.h"

#include <cassert>
#include <cstdio>

#include "opcode.h"
#include "value.h"

namespace js
{
void Chunk::write(u8 byte, int line)
{
	code.push_back(byte);
	lines.push_back(line);
}

size_t Chunk::add_constant(Value value)
{
	constants.push_back(value);
	return constants.size() - 1;
}

size_t Chunk::size() const
{
	return code.size();
}

u32 Chunk::allocate_register()
{
	assert(next_register != REG_MAX);
	return next_register++;
}

void Chunk::disassemble(const char *name) const
{
	std::printf("== %s ==\n", name);
	size_t offset = 0;
	while (offset < code.size())
		offset = disassemble_instruction(offset);
	
	std::printf("\n");
}

size_t Chunk::disassemble_instruction(size_t offset) const
{
	std::printf("%04ld ", offset);
	if (offset != 0 && lines[offset] == lines[offset + 1])
		std::printf("   | ");
	else
		std::printf("%*d ", 4, lines[offset]);

	auto instruction = static_cast<Opcode>(code[offset]);
	switch (instruction)
	{
		case OP_RETURN:
			return simple_instruction("return", offset);
		case OP_CONSTANT:
      		return constant_instruction("constant", offset);
		case OP_LOAD:
			return load_instruction(offset);
		case OP_ADD:
			return binary_instruction("add", offset);
		default:
			std::printf("Unknown opcode: %d\n", instruction);
			return offset + 1;
	}
}

size_t Chunk::simple_instruction(const char *name, size_t offset) const
{
	std::printf("%s\n", name);
	return offset + 1;
}

size_t Chunk::constant_instruction(const char *name, size_t offset) const
{
	auto constant = code[offset + 1];
	std::printf("%-16s %4d ", name, constant);
	std::printf("%s\n", constants[constant].to_string().c_str());
	return offset + 2;
}

size_t Chunk::byte_instruction(const char *name, size_t offset) const
{
	auto slot = code[offset + 1];
	std::printf("%-16s %4d\n", name, slot);
	return offset + 2;
}

size_t Chunk::jump_instruction(const char *name, int sign, size_t offset) const
{
	auto jump = (uint16_t) (code[offset + 1] << 8) | code[offset + 2];
	std::printf("%-16s %4ld -> %ld\n", name, offset, offset + 3 + sign * jump);
	return offset + 3;
}

size_t Chunk::array_instruction(const char *name, size_t offset) const
{
	auto constant = code[offset + 1];
	auto constant2 = code[offset + 2];
	std::printf("%-16s %4d ", name, constant);
	std::printf("%s ", constants[constant].to_string().c_str());
	std::printf("%s\n", constants[constant2].to_string().c_str());
	return offset + 3;
}

size_t Chunk::load_instruction(size_t offset) const
{
	auto reg = code[offset + 1];
	auto constant = code[offset + 2];
	std::printf("%-16s r%d, %d ", "load", reg, constant);
	std::printf("%s\n", constants[constant].to_string().c_str());
	return offset + 3;
}

size_t Chunk::binary_instruction(const char *name, size_t offset) const
{
	std::printf("%-16s r%d, r%d, r%d\n", name, code[offset + 1], code[offset + 2], code[offset + 3]);
	return offset + 4;
}
}
