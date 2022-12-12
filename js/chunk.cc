#include "chunk.h"

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

void Chunk::disassemble(const char *name)
{
	std::printf("== %s ==\n", name);
	size_t offset = 0;
	while (offset < code.size())
		offset = disassemble_instruction(offset);
	
	std::printf("\n");
}

size_t Chunk::size()
{
	return code.size();
}

size_t Chunk::disassemble_instruction(size_t offset)
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
			return simple_instruction("OP_RETURN", offset);
		case OP_CONSTANT:
      		return constant_instruction("OP_CONSTANT", offset);
		case OP_NULL:
			return simple_instruction("OP_NULL", offset);
		case OP_UNDEFINED:
			return simple_instruction("OP_UNDEFINED", offset);
		case OP_TRUE:
			return simple_instruction("OP_TRUE", offset);
		case OP_FALSE:
			return simple_instruction("OP_FALSE", offset);
		case OP_POP:
			return simple_instruction("OP_POP", offset);
		case OP_GET_LOCAL:
			return byte_instruction("OP_GET_LOCAL", offset);
		case OP_SET_LOCAL:
			return byte_instruction("OP_SET_LOCAL", offset);
		case OP_DEFINE_GLOBAL:
			return constant_instruction("OP_DEFINE_GLOBAL", offset);
		case OP_GET_GLOBAL:
			return constant_instruction("OP_GET_GLOBAL", offset);
		case OP_SET_GLOBAL:
			return constant_instruction("OP_SET_GLOBAL", offset);
		case OP_EQUAL:
			return simple_instruction("OP_EQUAL", offset);
		case OP_GREATER:
			return simple_instruction("OP_GREATER", offset);
		case OP_LESS:
			return simple_instruction("OP_LESS", offset);
		case OP_ADD:
			return simple_instruction("OP_ADD", offset);
		case OP_SUBTRACT:
			return simple_instruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simple_instruction("OP_MULTIPLY", offset);
		case OP_DIVIDE:
			return simple_instruction("OP_DIVIDE", offset);
		case OP_NOT:
			return simple_instruction("OP_NOT", offset);
		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", offset);
		case OP_JUMP:
			return jump_instruction("OP_JUMP", 1, offset);
		case OP_JUMP_IF_FALSE:
			return jump_instruction("OP_JUMP_IF_FALSE", 1, offset);
		case OP_LOOP:
			return jump_instruction("OP_LOOP", -1, offset);
		case OP_CALL:
			return byte_instruction("OP_CALL", offset);
		case OP_NEW_ARRAY:
			return byte_instruction("OP_NEW_ARRAY", offset);
		case OP_GET_SUBSCRIPT:
			return simple_instruction("OP_GET_SUBSCRIPT", offset);
		case OP_SET_SUBSCRIPT:
			return simple_instruction("OP_SET_SUBSCRIPT", offset);
		case OP_CLASS:
			return constant_instruction("OP_CLASS", offset);
		case OP_GET_PROPERTY:
			return constant_instruction("OP_GET_PROPERTY", offset);
		case OP_SET_PROPERTY:
			return constant_instruction("OP_SET_PROPERTY", offset);
		case OP_NEW_OBJECT:
			return new_object_instruction("OP_NEW_OBJECT", offset);
		case OP_PUSH_EXCEPTION:
			return jump_instruction("OP_PUSH_EXCEPTION", 1, offset);
		case OP_POP_EXCEPTION:
			return simple_instruction("OP_POP_EXCEPTION", offset);
		default:
			std::printf("Unknown opcode: %d\n", instruction);
			return offset + 1;
	}
}

size_t Chunk::simple_instruction(const char *name, size_t offset)
{
	std::printf("%s\n", name);
	return offset + 1;
}

size_t Chunk::constant_instruction(const char *name, size_t offset)
{
	auto constant = code[offset + 1];
	std::printf("%-16s %4d ", name, constant);
	std::printf("%s\n", constants[constant].to_string().c_str());
	return offset + 2;
}

size_t Chunk::byte_instruction(const char *name, size_t offset)
{
	auto slot = code[offset + 1];
	std::printf("%-16s %4d\n", name, slot);
	return offset + 2;
}

size_t Chunk::jump_instruction(const char *name, int sign, size_t offset)
{
	auto jump = (uint16_t) (code[offset + 1] << 8) | code[offset + 2];
	std::printf("%-16s %4ld -> %ld\n", name, offset, offset + 3 + sign * jump);
	return offset + 3;
}

size_t Chunk::new_object_instruction(const char *name, size_t offset)
{
	u8 num_properties = code[offset + 1];
	std::printf("%-16s %4d\n", name, num_properties);
	return offset + 2 + num_properties;
}
}
