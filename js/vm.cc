#include "vm.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

#include <fmt/format.h>

#include "chunk.h"
#include "object.h"
#include "opcode.h"

namespace js
{
/**
 * @brief initialize the Vm
 * @param headless if true, the Vm will not construct global variables
 * that are present in the javascript browser environment (i.e. window)
 * 
*/
Vm::Vm(bool headless)
{
	global = new Object();
	global->set("window", Value(global));

	auto *console = new Object();
	console->set_native("log", [](auto &vm, const auto &argv) -> Value
	{
		if (argv.empty())
			return {};

		for (uint i = 0; i < argv.size(); i++)
		{
			std::cout << argv[i].to_string();
			if (i != argv.size() - 1)
				std::cout << " ";
		}
		
		std::cout << "\n";
		return {};
	});
	
	global->set("console", Value(console));
}

bool Vm::run(Function f)
{
	auto cf = CallFrame { f, 0 };
	frames.push(cf);
	push(Value(&f));

	while (1)
	{
		auto instruction = static_cast<Opcode>(read_byte());

		switch (instruction)
		{
			case OP_RETURN:
			{
				auto result = pop();
				auto frame = frames.top();
				frames.pop();

				if (frames.empty())
					return true;
				
				auto diff = stack.size() - frame.base;
				for (uint i = 0; i < diff; i++)
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
			
			case OP_POP:
				pop();
				break;

			case OP_DEFINE_GLOBAL:
			{
				auto ident = read_string();
				global->set(ident, pop());
				break;
			}

			case OP_GET_GLOBAL:
			{
				auto ident = read_string();
				if (!global->is_defined(ident))
				{
					if (!runtime_error(fmt::format("Undefined variable '{}'", ident)))
						return false;
					break;
				}

				push(global->get(ident));
				break;
			}

			case OP_SET_GLOBAL:
			{
				auto ident = read_string();
				if (!global->is_defined(ident))
				{
					if (!runtime_error(fmt::format("Undefined variable '{}'", ident)))
						return false;
					break;
				}

				global->set(ident, peek(0));
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
				auto callee = peek(num_args);

				if (callee.is_native())
				{
					int i = num_args;
					std::vector<Value> argv;

					while (i--)
						argv.push_back(peek(i));

					auto result = callee.as_native()->call(*this, argv);
					for (int i = 0; i < num_args + 1; i++)
						pop();
					
					push(result);
				}

				else if (callee.is_function())
				{
					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame { *callee.as_function(), base };
					frames.push(cf);
				}

				else
				{
					assert(!"Tried to call an uncallable object");
				}
				
				break;
			}

			case OP_NEW_ARRAY:
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

				if (!array_value.is_array())
				{
					if (!runtime_error("Error: value is not an array"))
						return false;
					break;
				}

				auto array = array_value.as_array();

				if (!index.is_number())
				{
					if (!runtime_error("Error: array index is not a number"))
						return false;
					break;
				}

				int idx = (int) index.as_number();
				if (idx < 0 || idx >= (int) array->size())
				{
					if (!runtime_error(fmt::format("Error: array index {} out of bounds (length {})", idx, array->size())))
						return false;
					break;
				}

				push(Value(array->at(idx)));
				break;
			}

			case OP_SET_SUBSCRIPT:
			{
				auto element = pop();
				auto index = pop();
				auto array_value = pop();

				if (!array_value.is_array())
				{
					if (!runtime_error("Error: value is not an array"))
						return false;
					break;
				}

				auto array = array_value.as_array();

				if (!index.is_number())
				{
					if (!runtime_error("Error: array index is not a number"))
						return false;
					break;
				}

				int idx = (int) index.as_number();
				if (idx < 0 || idx >= (int) array->size())
				{
					if (!runtime_error(fmt::format("Error: array index {} out of bounds (length {})", idx, array->size())))
						return false;
					break;
				}

				array->at(idx) = element;
				push(element);
				break;
			}

			//case OP_CLASS:
			//{
			//	auto name = read_constant();
			//	auto klass = std::make_shared<Klass>(name.as_string());
			//	push(std::make_shared<Value>(klass));
			//	break;
			//}

			case OP_GET_PROPERTY:
			{
				if (!peek().is_object())
				{
					if (!runtime_error("Error: tried to get property on a non-object"))
						return false;
					break;
				}

				auto *obj = peek().as_object();
				auto val = obj->get(read_string());
				pop();
				push(val);
				break;
			}

			case OP_SET_PROPERTY:
			{
				if (!peek(1).is_object())
				{
					if (!runtime_error("Error: tried to get property on a non-object"))
						return false;
					break;
				}

				auto *obj = peek(1).as_object();
				obj->set(read_string(), peek());
				auto value = pop();
				push(value);
				pop();
				break;
			}

			case OP_NEW_OBJECT:
			{
				Object *obj = new Object();
				auto property_count = read_byte();

				for (int i = property_count - 1; i >= 0; i--)
				{
					auto key = read_string();
					auto value = peek(i);
					obj->set(key, value);
				}

				while (property_count--)
					pop();

				push(Value(obj));
				break;
			}

			case OP_PUSH_EXCEPTION:
			{
				auto offset = read_short();
				catchv.push_back(offset);
				break;
			}

			case OP_POP_EXCEPTION:
			{
				assert(!catchv.empty());
				catchv.pop_back();
				break;
			}

			case OP_THROW:
			{
				pop();
				break;
			}

			default:
				assert(!"Unknown opcode");
		}

		#ifdef DEBUG_PRINT_STACK
		std::printf("stack:          ");
		std::printf("[ ");
		for (auto &x : stack)
			std::printf("%s ", x.to_string().c_str());
		std::printf("]\n");
		#endif
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
		if (!runtime_error("Operands must be numbers"))
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

std::string Vm::read_string()
{
	auto constant = read_constant();
	assert(constant.is_string());
	return *constant.as_string();
}

bool Vm::runtime_error(std::string const &msg)
{
	if (catchv.empty())
	{
		auto ip = frames.top().ip;
		auto line = frames.top().function.chunk.lines[ip - 1];
		fmt::print("[line {}] Uncaught {}\n", line, msg);
		return false;
	}
	
	auto ip = catchv.back();
	frames.top().ip = ip;
	return true;
}
}
