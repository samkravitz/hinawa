#include "chunk.h"

#include <cstdio>

#include "opcode.h"
#include "value.h"

namespace js
{
void Chunk::write(u8 byte, int line)
{
	m_code.push_back(byte);
	m_lines.push_back(line);
}

size_t Chunk::add_constant(Value value)
{
	m_constants.push_back(value);
	return m_constants.size() - 1;
}

void Chunk::disassemble(const char *name) const
{
	std::printf("== %s ==\n", name);
	size_t offset = 0;
	while (offset < m_code.size())
		offset = disassemble_instruction(offset);
	
	std::printf("\n");
}

size_t Chunk::size() const
{
	return m_code.size();
}

size_t Chunk::disassemble_instruction(size_t offset) const
{
	std::printf("%04ld ", offset);
	if (offset != 0 && m_lines[offset] == m_lines[offset + 1])
		std::printf("   | ");
	else
		std::printf("%*d ", 4, m_lines[offset]);

	auto instruction = static_cast<Opcode>(m_code[offset]);
	switch (instruction)
	{
		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);
		case OP_CONSTANT:
      		return constant_instruction("OP_CONSTANT", offset);
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
	auto constant = m_code[offset + 1];
	std::printf("%-16s %4d ", name, constant);
	std::printf("%s\n", m_constants[constant].to_string().c_str());
	return offset + 2;
}

size_t Chunk::byte_instruction(const char *name, size_t offset) const
{
	auto slot = m_code[offset + 1];
	std::printf("%-16s %4d\n", name, slot);
	return offset + 2;
}

size_t Chunk::jump_instruction(const char *name, int sign, size_t offset) const
{
	auto jump = (uint16_t) (m_code[offset + 1] << 8) | m_code[offset + 2];
	std::printf("%-16s %4ld -> %ld\n", name, offset, offset + 3 + sign * jump);
	return offset + 3;
}

size_t Chunk::array_instruction(const char *name, size_t offset) const
{
	auto constant = m_code[offset + 1];
	auto constant2 = m_code[offset + 2];
	std::printf("%-16s %4d ", name, constant);
	std::printf("%s ", m_constants[constant].to_string().c_str());
	std::printf("%s\n", m_constants[constant2].to_string().c_str());
	return offset + 3;
}
}
