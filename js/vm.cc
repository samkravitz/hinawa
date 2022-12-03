#include "vm.h"

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
			{
				auto dest = read_byte();
				auto src1 = read_byte();
				auto src2 = read_byte();
				auto res = reg[src1].as_number() + reg[src2].as_number();
				reg[dest] = Value(res);
				break;
			}
		}
	}

done:
	std::cout << "Registers: \n";
	for (int i = 0; i < reg.size(); i++)
		std::printf("[r%d]: %s\n", i, reg[i].to_string().c_str());
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
