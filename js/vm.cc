#include "vm.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

#include <fmt/format.h>

#include "array.h"
#include "bindings/document_wrapper.h"
#include "chunk.h"
#include "compiler.h"
#include "document/document.h"
#include "heap.h"
#include "object.h"
#include "object_string.h"
#include "opcode.h"
#include "parser.h"
#include "prelude.h"
#include "string.hh"

#ifdef DEBUG_PRINT_AST
	#include "ast_printer.h"
#endif

namespace js
{
Vm::Vm() :
    Vm(nullptr)
{ }

Vm::Vm(Document *document)
{
	prelude(*this, document);
	heap().set_vm(*this);
}

Document &Vm::document()
{
	return m_document_wrapper->document();
}

void Vm::interpret(const std::string &program_string)
{
	auto program = Parser::parse(program_string);

#ifdef DEBUG_PRINT_AST
	auto printer = js::AstPrinter{};
	printer.print(program);
#endif

	auto *fn = Compiler::compile(program);
	run(*fn);
}

void Vm::run(Function &f)
{
	push(Value(&f));
	auto *closure = Closure::create(&f);
	auto cf = CallFrame{closure, 0};
	frames.push_back(cf);
	pop();
	push(Value(closure));

	while (1)
	{
		run_instruction(false);
		if (has_error() || should_return)
			break;
	}
}

void Vm::call(Closure *closure)
{
	auto num_args = closure->function->arity;
	auto base = static_cast<unsigned>(stack.size() - num_args - 1);
	auto cf = CallFrame{closure, base};
	frames.push_back(cf);

	while (1)
	{
		run_instruction(true);
		if (has_error() || should_return)
			break;
	}

	should_return = false;
	m_error = nullptr;
}

void Vm::run_instruction(bool in_call)
{
#ifdef DEBUG_PRINT_STACK
	frames.back().closure->function->chunk.disassemble_instruction(frames.back().ip);
#endif

	auto instruction = static_cast<Opcode>(read_byte());

	switch (instruction)
	{
		case OP_RETURN:
		{
			auto result = pop();
			auto frame = frames.back();
			Object *new_object = nullptr;
			if (frame.is_constructor)
				new_object = stack[frame.base].as_object();

			frames.pop_back();

			if (frames.empty() || in_call)
			{
				push(result);
				should_return = true;
				return;
			}

			auto diff = stack.size() - frame.base;
			for (uint i = 0; i < diff; i++)
				pop();

			if (new_object)
				push(Value(new_object));
			else
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

		case OP_INCREMENT:
		{
			auto value = peek();
			push(Value(value.as_number() + 1));
			break;
		}

		case OP_DECREMENT:
		{
			auto value = peek();
			push(Value(value.as_number() - 1));
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

		case OP_MOD:
			binary_op(Operator::Mod);
			break;

		case OP_NULL:
			push(Value(nullptr));
			break;

		case OP_UNDEFINED:
			push({});
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
			push(Value(a.eq(b)));
			break;
		}

		case OP_STRICT_EQUAL:
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
			const auto &ident = read_string();
			global->set(ident, pop());
			break;
		}

		case OP_GET_GLOBAL:
		{
			const auto &ident = read_string();
			if (!global->has_own_property(ident))
			{
				if (!runtime_error(heap().allocate<ReferenceError>(),
				                   fmt::format("Undefined variable '{}'", ident.string())))
					return;
				break;
			}

			push(global->get(ident));
			break;
		}

		case OP_SET_GLOBAL:
		{
			const auto &ident = read_string();
			global->set(ident, peek(0));
			break;
		}

		case OP_GET_LOCAL:
		{
			auto base = frames.back().base;
			auto slot = read_byte();
			push(stack[base + slot]);
			break;
		}

		case OP_SET_LOCAL:
		{
			auto base = frames.back().base;
			auto slot = read_byte();
			stack[base + slot] = peek();
			break;
		}

		case OP_JUMP_IF_FALSE:
		{
			auto offset = read_short();
			if (peek().is_falsy())
				frames.back().ip += offset;

			break;
		}

		case OP_JUMP:
		{
			auto offset = read_short();
			frames.back().ip += offset;
			break;
		}

		case OP_LOOP:
		{
			auto offset = read_short();
			frames.back().ip -= offset;
			break;
		}

		case OP_CALL:
		{
			auto num_args = read_byte();
			auto callee = peek(num_args);

			if (callee.is_object())
			{
				Object *obj = callee.as_object();
				Object *method = obj, *receiver = obj;

				if (obj->is_closure())
				{
					auto *closure = obj->as_closure();
					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame{closure, base};
					frames.push_back(cf);
					stack[base] = Value(receiver);
					break;
				}

				if (obj->is_bound_method())
				{
					auto *bound = static_cast<BoundMethod *>(obj);
					method = bound->method;
					receiver = bound->receiver;
				}

				if (method->is_native())
				{
					int i = num_args;
					std::vector<Value> argv;

					while (i--)
						argv.push_back(peek(i));

					auto result = method->as_native()->call(*this, argv);
					for (int i = 0; i < num_args + 1; i++)
						pop();

					push(result);
				}

				else
				{
					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame{method->as_closure(), base};
					frames.push_back(cf);
					stack[base] = Value(receiver);
				}
			}

			else
			{
				assert(!"Tried to call an uncallable object");
			}

			break;
		}

		case OP_CALL_CONSTRUCTOR:
		{
			auto num_args = read_byte();
			auto callee = peek(num_args);

			if (callee.is_object())
			{
				auto *obj = callee.as_object();

				if (obj->is_closure())
				{
					auto *constructor = obj->as_closure();
					int arity = constructor->function->arity;
					if (num_args < arity)
					{
						for (int i = 0; i < arity - num_args; i++)
							push({});

						num_args = arity;
					}

					auto *prototype = constructor->get("prototype").as_object();
					Object *new_object = heap().allocate();
					new_object->set_prototype(prototype);

					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame{constructor->as_closure(), base};
					cf.is_constructor = true;
					frames.push_back(cf);
					stack[base] = Value(new_object);
				}

				else if (obj->is_native())
				{
					auto *native = obj->as_native();
					int i = num_args;
					std::vector<Value> argv;

					while (i--)
						argv.push_back(peek(i));

					auto result = native->call(*this, argv);
					for (int i = 0; i < num_args + 1; i++)
						pop();

					push(result);
				}
			}

			else
			{
				assert(!"Ivalid new expression with an uncallable object");
			}

			break;
		}

		case OP_NEW_ARRAY:
		{
			auto num_elements = read_byte();
			std::vector<Value> array;
			while (num_elements--)
				array.push_back(peek(num_elements));

			for (const auto &element : array)
			{
				std::ignore = element;
				pop();
			}

			//std::reverse(array->begin(), array->end());
			push(Value(heap().allocate<Array>(array)));
			break;
		}

		case OP_GET_SUBSCRIPT:
		{
			auto index = pop();
			auto array_value = pop();

			if (!array_value.is_object() || !array_value.as_object()->is_array())
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: value is not an array"))
					return;
				break;
			}

			auto array = array_value.as_object()->as_array();

			if (!index.is_number())
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: array index is not a number"))
					return;
				break;
			}

			int idx = (int) index.as_number();
			if (idx < 0 || idx >= (int) array->size())
			{
				if (!runtime_error(heap().allocate<TypeError>(),
				                   fmt::format("Error: array index {} out of bounds (length {})", idx, array->size())))
					return;
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

			if (!array_value.is_object() || !array_value.as_object()->is_array())
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: value is not an array"))
					return;
				break;
			}

			auto array = array_value.as_object()->as_array();

			if (!index.is_number())
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: array index is not a number"))
					return;
				break;
			}

			int idx = (int) index.as_number();
			if (idx < 0 || idx >= (int) array->size())
			{
				if (!runtime_error(heap().allocate<TypeError>(),
				                   fmt::format("Error: array index {} out of bounds (length {})", idx, array->size())))
					return;
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
			Object *obj;

			if (peek().is_string())
			{
				obj = heap().allocate<ObjectString>(peek().as_string());
			}

			else if (peek().is_object())
			{
				obj = peek().as_object();
			}

			else
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: tried to get property on a non-object"))
					return;
				break;
			}

			_this = obj;
			auto val = obj->get(read_string());
			if (val.is_object() && val.as_object()->is_closure())
				val = Value(heap().allocate<BoundMethod>(obj, val.as_object()->as_closure()));

			pop();
			push(val);
			break;
		}

		case OP_SET_PROPERTY:
		{
			if (!peek(1).is_object())
			{
				if (!runtime_error(heap().allocate<TypeError>(), "Error: tried to get property on a non-object"))
					return;
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
			Object *obj = heap().allocate();
			auto property_count = read_byte();

			for (int i = property_count - 1; i >= 0; i--)
			{
				const auto &key = read_string();
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
			frames.back().catchv.push_back({frames.back().ip + offset, stack.size()});
			break;
		}

		case OP_POP_EXCEPTION:
		{
			assert(!frames.back().catchv.empty());
			frames.back().catchv.pop_back();
			break;
		}

		case OP_THROW:
		{
			bool caught = false;
			auto val = pop();
			while (!frames.empty())
			{
				auto &frame = frames.back();

				if (frame.catchv.empty())
				{
					frames.pop_back();
					continue;
				}

				auto catch_env = frame.catchv.back();
				assert(catch_env.sp <= stack.size());
				while (stack.size() != catch_env.sp)
					pop();

				frame.ip = catch_env.ip;
				caught = true;
				break;
			}

			if (!caught)
				fmt::print(stderr, "Uncaught Exception!\n");

			push(val);
			break;
		}

		case OP_CLOSURE:
		{
			auto *function = read_constant().as_object()->as_function();
			auto *closure = Closure::create(function);
			push(Value(closure));

			for (int i = 0; i < function->upvalue_count; i++)
			{
				auto index = read_byte();
				auto is_local = read_byte();
				if (is_local)
					closure->upvalues.push_back(capture_upvalue(&stack[frames.back().base + index]));
				else
					closure->upvalues.push_back(frames.back().closure->upvalues[index]);
			}
			break;
		}

		case OP_GET_UPVALUE:
		{
			auto slot = read_byte();
			push(*frames.back().closure->upvalues[slot]->location);
			break;
		}

		case OP_SET_UPVALUE:
		{
			auto slot = read_byte();
			*frames.back().closure->upvalues[slot]->location = peek();
			break;
		}

		case OP_INSTANCEOF:
		{
			auto constructor = peek();
			auto obj = peek(1);

			// TODO - this should be a runtime error.
			if (!obj.is_object() || !constructor.is_object())
			{
				fmt::print(stderr, "{} [instanceof] {} are not objects!\n", peek().to_string(), peek(1).to_string());
				pop();
				pop();
				push(Value(false));
				break;
			}

			auto result = Value(false);
			if (obj.as_object()->get("constructor") == constructor)
				result = Value(true);

			pop();
			pop();
			push(result);
			break;
		}

		case OP_TYPEOF:
		{
			auto val = peek();
			auto typeof_val = Value(heap().allocate_string(val.type_of()));
			pop();
			push(typeof_val);
			break;
		}

		default:
			assert(!"Unknown opcode");
	}

#ifdef DEBUG_PRINT_STACK
	print_stack();
#endif
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
	Value result = {};
	auto b = pop();
	auto a = pop();

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
			result = a % b;
			break;

		case Operator::LessThan:
			result = a < b;
			break;

		case Operator::GreaterThan:
			result = a > b;
			break;

		case Operator::Amp:
			result = a & b;
			break;

		case Operator::AmpAmp:
			result = a && b;
			break;

		case Operator::Pipe:
			result = a | b;
			break;

		case Operator::PipePipe:
			result = a || b;
			break;

		default:
			assert(!"Unreachable");
	}

	push(result);
}

u8 Vm::read_byte()
{
	auto ip = frames.back().ip;
	auto byte = frames.back().closure->function->chunk.code[ip];
	frames.back().ip += 1;
	return byte;
}

u16 Vm::read_short()
{
	auto ip = frames.back().ip;
	auto a = frames.back().closure->function->chunk.code[ip];
	auto b = frames.back().closure->function->chunk.code[ip + 1];
	frames.back().ip += 2;

	return (a << 8) | b;
}

Value Vm::read_constant()
{
	auto byte = read_byte();
	return frames.back().closure->function->chunk.constants[byte];
}

String &Vm::read_string()
{
	auto constant = read_constant();
	assert(constant.is_string());
	return constant.as_string();
}

Upvalue *Vm::capture_upvalue(Value *local)
{
	return heap().allocate<Upvalue>(local);
}

void Vm::print_stack() const
{
	fmt::print("stack:          ");
	fmt::print("[ ");
	for (auto &x : stack)
		fmt::print("{} ", x.to_string());
	fmt::print("]\n");
}

bool Vm::runtime_error(Error *err, std::string const &msg)
{
	if (frames.back().catchv.empty())
	{
		m_error = err;
		auto ip = frames.back().ip;
		auto line = frames.back().closure->function->chunk.lines[ip - 1];
		fmt::print("[line {}] Uncaught {}\n", line, msg);
		return false;
	}

	m_error = nullptr;
	auto ip = frames.back().catchv.back().ip;
	frames.back().ip = ip;
	return true;
}
}
