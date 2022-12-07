#include "vm.h"

#include <cassert>
#include <cstdio>
#include <iostream>

#include "opcode.h"

namespace js
{
Value Vm::run(Chunk const &chunk)
{
	this->chunk = chunk;
	reg.resize(chunk.reg_count());

	while (1)
	{
		auto instruction = static_cast<Opcode>(read_byte());

		switch (instruction)
		{
			case OP_RETURN:
			{
				goto done;
			}

			case OP_LOAD:
			{
				auto dest = read_byte();
				auto val = read_constant();
				reg[dest] = val;
				break;
			}

			case OP_ADD:
				binary_op(Operator::Plus);
				break;
			
			case OP_SUBTRACT:
				binary_op(Operator::Minus);
				break;
			
			case OP_MULTIPLY:
				binary_op(Operator::Star);
				break;
			
			case OP_DIVIDE:
				binary_op(Operator::Slash);
				break;

			default:
			{
				std::cout << "Unknown instruction: " << (int) instruction << "\n";
				goto done;
			}
		}
	}

done:
	std::cout << "Registers: \n";
	for (auto i = 0; i < reg.size(); i++)
		std::printf("[r%d]: %s\n", i, reg[i].to_string().c_str());
}

void Vm::binary_op(Operator op)
{
	auto dest = read_byte();
	auto a = reg[read_byte()].as_number();
	auto b = reg[read_byte()].as_number();

	double result;
	bool x;

	switch (op)
	{
		case Operator::Plus:
			result = a + b;
			break;

		case Operator::Minus:
			result = a - b;
			break;
		
		case Operator::Star:
			result = a * b;
			break;

		case Operator::Slash:
			result = a / b;
			break;

		case Operator::Mod:
			result = (int) a % (int) b;
			break;
		
		case Operator::LessThan:
			x = a < b;
			break;
		
		case Operator::GreaterThan:
			x = a > b;
			break;
		
		case Operator::Amp:
			x = (int) a & (int) b;
			break;

		case Operator::AmpAmp:
			x = (int) a && (int) b;
			break;
		
		case Operator::Pipe:
			x = (int) a | (int) b;
			break;
		
		case Operator::PipePipe:
			x = (int) a || (int) b;
			break;
		
		default:
			assert(!"Unreachable");
	}

	reg[dest] = Value(result);
}

u8 Vm::read_byte()
{
	auto byte = chunk.code[ip];
	ip += 1;
	return byte;
}

u16 Vm::read_short()
{
	auto a = chunk.code[ip];
	auto b = chunk.code[ip + 1];
	ip += 2;

	return (a << 8) | b;
}

Value Vm::read_constant()
{
	auto byte = read_byte();
	return chunk.constants[byte];
}
}
