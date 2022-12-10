#include "vm.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

#include "chunk.h"
#include "opcode.h"

namespace js
{
Value Vm::run(Function f)
{
	auto cf = CallFrame { f, 0 };
	frames.push(cf);

	while (1)
	{
		#ifdef DEBUG
		std::printf("stack:          ");
		std::printf("[ ");
		for (auto &x : stack)
			std::printf("%s ", x.to_string().c_str());
		std::printf("]\n");
		#endif

		auto instruction = static_cast<Opcode>(read_byte());

		switch (instruction)
		{
			case OP_RETURN:
			{
				auto result = pop();
				auto frame = frames.top();
				frames.pop();

				if (frames.empty())
					return result;
				
				auto diff = stack.size() - frame.base;
				for (int i = 0; i < diff; i++)
					pop();

				push(result);
				break;
			}
			
			case OP_CONSTANT:
			{
				auto constant = read_constant();
				push(constant);
				break;
			}

			case OP_NEGATE:
				// TODO
				break;

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
			
			case OP_MOD:
				binary_op(Operator::Mod);
				break;
			
			case OP_NULL:
				push(Value(Value::Type::Null));
				break;

			case OP_UNDEFINED:
				push(Value());
				break;
			
			case OP_TRUE:
				push(Value(true));
				break;
			
			case OP_FALSE:
				push(Value(false));
				break;
			
			case OP_NOT:
			{
				auto value = pop();
				push(Value(value.is_falsy()));
				break;
			}
			
			case OP_EQUAL:
			{
				auto b = pop();
				auto a = pop();
				push(Value(a == b));
				break;
			}
			
			case OP_GREATER:
				binary_op(Operator::GreaterThan);
				break;
			
			case OP_LESS:
				binary_op(Operator::LessThan);
				break;
			
			case OP_LOGICAL_AND:
				binary_op(Operator::AmpAmp);
				break;

			case OP_LOGICAL_OR:
				binary_op(Operator::PipePipe);
				break;
			
			case OP_BITWISE_AND:
				binary_op(Operator::Amp);
				break;

			case OP_BITWISE_OR:
				binary_op(Operator::Pipe);
				break;
			
			case OP_PRINT:
				std::cout << peek().to_string() << "\n";
				break;
			
			case OP_POP:
				pop();
				break;
			
			case OP_GET_GLOBAL:
			{
				auto constant = read_constant();
				auto variable = globals.find(*constant.as_string());

				if (variable != globals.end())
					push(variable->second);
				else
					runtime_error("Undefined variable");
				break;
			}

			case OP_SET_GLOBAL:
			{
				auto constant = read_constant();
				std::string name;
				Value value;
				if (constant.is_string())
				{
					name = *constant.as_string();
					value = peek();
				}

				else if (constant.is_function())
				{
					name = constant.as_function()->name;
					value = Value(constant.as_function());
				}

				else
					assert(!"Error: got unknown type in OP_SET_GLOBAL");

				globals[name] = value;
				break;
			}

			case OP_GET_LOCAL:
			{
				auto base = frames.top().base;
				auto slot = read_byte();
				push(stack[base + slot]);
				break;
			}

			case OP_SET_LOCAL:
			{
				auto base = frames.top().base;
				auto slot = read_byte();
				stack[base + slot] = peek();
				break;
			}

			case OP_JUMP_IF_FALSE:
			{
				auto offset = read_short();
				if (peek().is_falsy())
					frames.top().ip += offset;
				
				break;
			}

			case OP_JUMP:
			{
				auto offset = read_short();
				frames.top().ip += offset;
				break;
			}

			case OP_LOOP:
			{
				auto offset = read_short();
				frames.top().ip -= offset;
				break;
			}

			case OP_CALL:
			{
				auto num_args = read_byte();
				auto callable = peek(num_args);

				if (callable.is_function())
				{
					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame { *callable.as_function(), base };
					frames.push(cf);
				}

				else
				{
					assert(!"Tried to call an uncallable object");
				}
				
				break;
			}

			case OP_BUILD_ARRAY:
			{
				auto num_elements = read_byte();
				std::vector<Value> *array = new std::vector<Value>;
				while (num_elements--)
					array->push_back(pop());
				
				std::reverse(array->begin(), array->end());
				push(Value(array));
				break;
			}

			case OP_GET_SUBSCRIPT:
			{
				auto index = pop();
				auto array_value = pop();

				auto array = *array_value.as_array();
				push(Value(array[(int) index.as_number()]));

				break;
			}

			//case OP_SET_SUBSCRIPT:
			//{
			//	auto element = pop();
			//	auto index = pop();
			//	auto array = pop();

			//	array->store_at((int) index->as_number(), *element);
			//	push(element);
			//	break;
			//}

			//case OP_CLASS:
			//{
			//	auto name = read_constant();
			//	auto klass = std::make_shared<Klass>(name.as_string());
			//	push(std::make_shared<Value>(klass));
			//	break;
			//}

			//case OP_GET_PROPERTY:
			//{
			//	auto instance = peek()->as_instance();
			//	auto name = read_constant().as_string();

			//	// if an instance's value isn't set, default to nil
			//	auto property = std::make_shared<Value>(nullptr);

			//	if (instance->fields.find(name) != instance->fields.end())
			//		property = instance->fields[name];

			//	push(property);
			//	break;
			//}

			//case OP_SET_PROPERTY:
			//{
			//	auto instance = peek(1)->as_instance();
			//	auto name = read_constant().as_string();

			//	auto property = pop();
			//	instance->fields[name] = property;
			//	pop();
			//	push(property);
			//	break;
			//}

			default:
				assert(!"Unknown opcode");
		}
	}
}

void Vm::push(Value value)
{
	stack.push_back(value);
}

Value Vm::pop()
{
	auto value = stack.back();
	stack.pop_back();
	return value;
}

Value Vm::peek(uint offset)
{
	return stack[stack.size() - offset - 1];
}

void Vm::binary_op(Operator op)
{
	if (!peek(0).is_number() || !peek(1).is_number())
	{
		runtime_error("Operands must be numbers");
		exit(1);
		return;
	}

	double result;
	bool x;
	auto b = pop().as_number();
	auto a = pop().as_number();

	switch (op)
	{
		case Operator::Plus:
			result = a + b;
			push(Value(result));
			break;

		case Operator::Minus:
			result = a - b;
			push(Value(result));
			break;
		
		case Operator::Star:
			result = a * b;
			push(Value(result));
			break;

		case Operator::Slash:
			result = b / a;
			push(Value(result));
			break;

		case Operator::Mod:
			result = (int) a % (int) b;
			push(Value(result));
			break;
		
		case Operator::LessThan:
			x = a < b;
			push(Value(x));
			break;
		
		case Operator::GreaterThan:
			x = a > b;
			push(Value(x));
			break;
		
		case Operator::Amp:
			x = (int) a & (int) b;
			push(Value(x));
			break;

		case Operator::AmpAmp:
			x = (int) a && (int) b;
			push(Value(x));
			break;
		
		case Operator::Pipe:
			x = (int) a | (int) b;
			push(Value(x));
			break;
		
		case Operator::PipePipe:
			x = (int) a || (int) b;
			push(Value(x));
			break;
		
		default:
			assert(!"Unreachable");
	}
}

u8 Vm::read_byte()
{
	auto ip = frames.top().ip;
	auto byte = frames.top().function.chunk.code[ip];
	frames.top().ip += 1;
	return byte;
}

u16 Vm::read_short()
{
	auto ip = frames.top().ip;
	auto a = frames.top().function.chunk.code[ip];
	auto b = frames.top().function.chunk.code[ip + 1];
	frames.top().ip += 2;

	return (a << 8) | b;
}

Value Vm::read_constant()
{
	auto byte = read_byte();
	return frames.top().function.chunk.constants[byte];
}

void Vm::runtime_error(std::string const &msg)
{
	auto ip = frames.top().ip;
	auto line = frames.top().function.chunk.lines[ip - 1];
	std::printf("%s [line %d]\n", msg.c_str(), line);
}
}
