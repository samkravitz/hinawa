#include "chunk.h"

#include <cassert>
#include <cstdio>
#include <fmt/format.h>

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
	fmt::print("== {} ==\n", name);
	size_t offset = 0;
	while (offset < code.size())
		offset = disassemble_instruction(offset);

	fmt::print("\n");
}

size_t Chunk::size()
{
	return code.size();
}

size_t Chunk::disassemble_instruction(size_t offset)
{
	fmt::print("{:04} ", offset);
	if (offset != 0 && lines[offset] == lines[offset + 1])
		fmt::print("   | ");
	else
		fmt::print("{:4} ", lines[offset]);

	auto instruction = static_cast<Opcode>(code[offset]);
	switch (instruction)
	{
		case OP_LOADK:
		{
			auto dst = code[offset + 1];
			auto k = code[offset + 2];

			fmt::print("{:16} {:3} {:3} {:3} ", "OP_LOADK", dst, k, "");
			fmt::print("; r{} = {}\n", dst, constants[k].to_string());
			return offset + 3;
		}
		case OP_MOV:
		{
			auto dst = code[offset + 1];
			auto src = code[offset + 2];
			fmt::print("{:16} {:3} {:3} {:3} ", "OP_MOV", dst, src, "");
			fmt::print("; r{} = r{}\n", dst, src);
			return offset + 3;
		}
		case OP_RETURN:
		{
			auto dst = code[offset + 1];
			fmt::print("{0:16} {2:3} {1:3} {1:3} ; return r{2}\n", "OP_RETURN", "", dst);
			return offset + 2;
		}
		case OP_CONSTANT:
      		return constant_instruction("OP_CONSTANT", offset);
		case OP_INCREMENT:
      		return simple_instruction("OP_INCREMENT", offset);
		case OP_DECREMENT:
      		return simple_instruction("OP_DECREMENT", offset);
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
		{
			auto a = code[offset + 1];
			auto k = code[offset + 2];
			fmt::print("{:16} {:3} {:3} {:3} ", "OP_GET_GLOBAL", a, k, "");
			fmt::print("; r{} = {}\n", a, constants[k].to_string());
			return offset + 3;
		}
		case OP_SET_GLOBAL:
			return constant_instruction("OP_SET_GLOBAL", offset);
		case OP_EQUAL:
		case OP_GREATER:
		case OP_LESS:
		case OP_ADD:
		case OP_SUBTRACT:
		case OP_MULTIPLY:
		case OP_DIVIDE:
		case OP_NOT:
			return binary_instruction(instruction, offset);
		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", offset);
		case OP_JUMP:
			return jump_instruction("OP_JUMP", 1, offset);
		case OP_JUMP_IF_FALSE:
			return jump_instruction("OP_JUMP_IF_FALSE", 1, offset);
		case OP_LOOP:
			return jump_instruction("OP_LOOP", -1, offset);
		case OP_CALL:
		{
			auto dst = code[offset + 1];
			auto num_args = code[offset + 2];
			fmt::print("{0:16} {2:3} {1:3} {1:3} ; r{2} = r{2}()\n", "OP_CALL", "", dst);
			return offset + 3;
		}
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
		case OP_THROW:
			return simple_instruction("OP_THROW", offset);
		default:
			fmt::print("Unknown opcode: {}\n", instruction);
			return offset + 1;
	}
}

size_t Chunk::binary_instruction(Opcode op, size_t offset)
{
	const char *name, *op_str;
		switch (op)
	{
		case OP_ADD: name = "OP_ADD"; op_str = "+"; break;
		case OP_SUBTRACT: name = "OP_SUBTRACT"; op_str = "-"; break; 
		case OP_MULTIPLY: name = "OP_MULTIPLY"; op_str = "*"; break; 
		case OP_DIVIDE: name = "OP_DIVIDE"; op_str = "/"; break; 
		case OP_MOD: name = "OP_MOD"; op_str = "%"; break; 
		case OP_EQUAL: name = "OP_EQUAL"; op_str = "=="; break; 
		case OP_GREATER: name = "OP_GREATER"; op_str = ">"; break;
		case OP_LESS: name = "OP_LESS"; op_str = "<"; break; 
		case OP_BITWISE_AND: name = "OP_BITWISE_AND"; op_str = "&"; break; 
		case OP_BITWISE_OR: name = "OP_BITWISE_OR"; op_str = "|"; break; 
		default:
			assert(!"Unreachable");
	}

	auto dst = code[offset + 1];
	auto a = code[offset + 2];
	auto b = code[offset + 3];
	fmt::print("{:16} {:3} {:3} {:3} ; r{} = r{} {} r{}\n", name, dst, a, b, dst, a, op_str, b);
	return offset + 4;
}

size_t Chunk::simple_instruction(const char *name, size_t offset)
{
	auto dst = code[offset + 1];
	auto r1 = code[offset + 2];
	auto r2 = code[offset + 3];
	fmt::print("{:16} {:3} {:3} {:3} ; r{} = r{} + r{}\n", name, dst, r1, r2, dst, r1, r2);
	return offset + 4;
}

size_t Chunk::constant_instruction(const char *name, size_t offset)
{
	auto a = code[offset + 1];
	auto k = code[offset + 2];
	fmt::print("{:16} {:3} {:3} {:3} ", name, a, k, "");
	fmt::print("; {} = r{}\n", constants[k].to_string(), a);
	return offset + 3;
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
