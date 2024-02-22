#include "vm.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <ranges>
#include <unordered_set>

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

Heap &Vm::heap()
{
	return js::heap();
}

void Vm::interpret(const std::string &program_string)
{
	auto program = Parser::parse(program_string);

#ifdef DEBUG_PRINT_AST
	auto printer = js::AstPrinter{};
	printer.print(program);
#endif

	auto *fn = Compiler::compile(program);
	push(Value(fn));
	auto *closure = Closure::create(fn);
	auto cf = CallFrame{closure, 0};
	cf._this = m_global;
	pop();
	push(Value(closure));
	call(cf);
}

Object *Vm::current_this() const
{
	return static_cast<Object *>(call_stack.back()._this);
}

Value Vm::call(Closure *closure)
{
	auto num_args = closure->function->arity;
	auto base = static_cast<unsigned>(stack.size() - num_args - 1);
	auto cf = CallFrame{closure, base};
	call(cf);
	return pop();
}

Value Vm::call(const CallFrame &cf)
{
	call_stack.push_back(cf);
	bool should_return = false;

	while (1)
	{
		run_instruction(should_return);
		if (has_error() || should_return)
			break;
	}

	return peek();
}

void Vm::run_instruction(bool &should_return)
{
#ifdef DEBUG_PRINT_STACK
	call_stack.back().closure->function->chunk.disassemble_instruction(call_stack.back().ip);
#endif

	auto instruction = static_cast<Opcode>(read_byte());

	switch (instruction)
	{
		case OP_RETURN:
		{
			auto result = pop();
			auto call_frame = call_stack.back();
			auto *_this = current_this();
			call_stack.pop_back();

			auto diff = stack.size() - call_frame.base;
			for (unsigned i = 0; i < diff; i++)
				pop();

			if (call_frame.is_constructor)
				push(Value(_this));
			else
				push(result);

			should_return = true;
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
			m_global->set(ident, pop());
			break;
		}

		case OP_GET_GLOBAL:
		{
			const auto &ident = read_string();
			if (!m_global->has_own_property(ident))
			{
				if (!runtime_error(heap().allocate<ReferenceError>(*this, ident.string()),
				                   fmt::format("Undefined variable '{}'", ident.string())))
					return;
				break;
			}

			push(m_global->get(ident));
			break;
		}

		case OP_SET_GLOBAL:
		{
			const auto &ident = read_string();
			if (!m_global->has_own_property(ident))
			{
				pop();
				if (!runtime_error(heap().allocate<ReferenceError>(*this, ident.string()),
				                   fmt::format("Undefined variable '{}'", ident.string())))
					return;
				break;
			}

			m_global->set(ident, peek(0));
			break;
		}

		case OP_GET_LOCAL:
		{
			auto base = call_stack.back().base;
			auto slot = read_byte();

			Value value = stack[base + slot];

			// special case for `this`
			if (slot == 0)
				value = Value(current_this());

			push(value);
			break;
		}

		case OP_SET_LOCAL:
		{
			auto base = call_stack.back().base;
			auto slot = read_byte();
			stack[base + slot] = peek();
			break;
		}

		case OP_JUMP_IF_FALSE:
		{
			auto offset = read_short();
			if (peek().is_falsy())
				call_stack.back().ip += offset;

			break;
		}

		case OP_JUMP:
		{
			auto offset = read_short();
			call_stack.back().ip += offset;
			break;
		}

		case OP_LOOP:
		{
			auto offset = read_short();
			call_stack.back().ip -= offset;
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
					cf._this = receiver;
					call(cf);
					break;
				}

				if (obj->is_bound_method())
				{
					auto *bound = static_cast<BoundMethod *>(obj);
					method = bound->method;
					receiver = bound->receiver;
				}

				if (obj->is_bound_native_method())
				{
					auto *bound = static_cast<BoundNativeMethod *>(obj);
					method = bound->method;
					receiver = bound->receiver;
				}

				if (method->is_native())
				{
					int i = num_args;
					std::vector<Value> argv;

					while (i--)
						argv.push_back(peek(i));

					auto cf = CallFrame{0, 0};
					cf._this = receiver;
					call_stack.push_back(cf);
					auto result = method->as_native()->call(*this, argv);
					call_stack.pop_back();
					for (int i = 0; i < num_args + 1; i++)
						pop();

					push(result);
				}

				else
				{
					auto base = static_cast<uint>(stack.size() - num_args - 1);
					auto cf = CallFrame{method->as_closure(), base};
					cf._this = receiver;
					call(cf);
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
					cf._this = new_object;
					call(cf);
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

			auto val = obj->get(read_string());
			if (val.is_object())
			{
				if (val.as_object()->is_closure())
					val = Value(heap().allocate<BoundMethod>(obj, val.as_object()->as_closure()));
				else if (val.as_object()->is_native())
					val = Value(heap().allocate<BoundNativeMethod>(obj, val.as_object()->as_native()));
			}

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
			call_stack.back().unwind_contexts.push_back({call_stack.back().ip + offset, stack.size()});
			break;
		}

		case OP_POP_EXCEPTION:
		{
			assert(!call_stack.back().unwind_contexts.empty());
			call_stack.back().unwind_contexts.pop_back();
			break;
		}

		case OP_THROW:
		{
			if (!runtime_error(pop(), ""))
				return;

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
					closure->upvalues.push_back(capture_upvalue(&stack[call_stack.back().base + index]));
				else
					closure->upvalues.push_back(call_stack.back().closure->upvalues[index]);
			}
			break;
		}

		case OP_GET_UPVALUE:
		{
			auto slot = read_byte();
			push(*call_stack.back().closure->upvalues[slot]->location);
			break;
		}

		case OP_SET_UPVALUE:
		{
			auto slot = read_byte();
			*call_stack.back().closure->upvalues[slot]->location = peek();
			break;
		}

		case OP_INSTANCEOF:
		{
			auto constructor_value = peek();
			auto obj_value = peek(1);

			// TODO - this should be a runtime error.
			if (!obj_value.is_object() || !constructor_value.is_object())
			{
				fmt::print(stderr, "{} [instanceof] {} are not objects!\n", peek().to_string(), peek(1).to_string());
				pop();
				pop();
				push(Value(false));
				break;
			}

			auto *obj = obj_value.as_object();
			auto *constructor = constructor_value.as_object();
			auto constructor_prototype = constructor->get("prototype");

			bool result = false;
			for (auto *prototype = obj->prototype(); prototype; prototype = prototype->prototype())
			{
				if (Value(prototype) == constructor_prototype)
				{
					result = true;
					break;
				}
			}

			pop();
			pop();
			push(Value(result));
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

	if (call_stack.empty())
		should_return = true;

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
	auto b = pop();
	auto a = pop();

	static std::unordered_set<Operator> comparison_operators = {
	    Operator::LessThan,
	    Operator::GreaterThan,
	};

	static std::unordered_set<Operator> binary_operators = {
	    Operator::Plus,
	    Operator::Minus,
	    Operator::Slash,
	    Operator::Star,
	    Operator::StarStar,
	    Operator::Mod,
	    Operator::Amp,
	    Operator::Pipe,
	};

	static std::unordered_set<Operator> logical_operators = {
	    Operator::AmpAmp,
	    Operator::PipePipe,
	};

	std::expected<Value, Error *> result_or_error = {};

	if (comparison_operators.contains(op))
		result_or_error = apply_comparison_operator(*this, a, op, b);

	if (binary_operators.contains(op))
		result_or_error = apply_binary_operator(*this, a, op, b);

	if (logical_operators.contains(op))
		result_or_error = apply_logical_operator(*this, a, op, b);

	if (!result_or_error)
	{
		runtime_error(result_or_error.error(), "Binary op runtime error");
		return;
	}

	push(*result_or_error);
}

u8 Vm::read_byte()
{
	auto ip = call_stack.back().ip;
	auto byte = call_stack.back().closure->function->chunk.code[ip];
	call_stack.back().ip += 1;
	return byte;
}

u16 Vm::read_short()
{
	auto ip = call_stack.back().ip;
	auto a = call_stack.back().closure->function->chunk.code[ip];
	auto b = call_stack.back().closure->function->chunk.code[ip + 1];
	call_stack.back().ip += 2;

	return (a << 8) | b;
}

Value Vm::read_constant()
{
	auto byte = read_byte();
	return call_stack.back().closure->function->chunk.constants[byte];
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

bool Vm::runtime_error(Error *err, const std::string &msg)
{
	m_error = err;
	return runtime_error(Value(err), msg);
}

bool Vm::runtime_error(Value thrown_value, const std::string &msg)
{
	bool caught = false;
	auto call_stack_copy = call_stack;

	/**
	* Iterate through the call stack backwards, finding the first frame which contains
	* an unwind context. If no such frame exists, the error is uncaught
	*/
	while (!call_stack.empty())
	{
		auto &call_frame = call_stack.back();
		if (call_frame.unwind_contexts.empty())
		{
			call_stack.pop_back();
			continue;
		}

		auto catch_env = call_frame.unwind_contexts.back();
		assert(catch_env.sp <= stack.size());
		while (stack.size() != catch_env.sp)
			pop();

		call_frame.ip = catch_env.ip;
		caught = true;
		break;
	}

	if (!caught)
	{
		call_stack = call_stack_copy;
		fmt::print(stderr, "Uncaught exception:\n");
		print_stack_trace();
		m_error = heap().allocate<Error>();
		return false;
	}

	m_error = nullptr;
	push(thrown_value);
	return true;
}

void Vm::print_stack() const
{
	fmt::print("stack:          ");
	fmt::print("[ ");
	for (auto &x : stack)
		fmt::print("{} ", x.to_string());
	fmt::print("]\n");
}

void Vm::print_stack_trace() const
{
	fmt::print(stderr, "{}", stack_trace());
}

std::string Vm::stack_trace() const
{
	std::string stacktrace = "";
	for (const auto &call_frame : call_stack | std::views::reverse)
	{
		auto *function = call_frame.closure->function;
		auto instruction = call_frame.ip - 1;
		stacktrace +=
		    fmt::format("at {}: line {}\n", function->name_for_stack_trace(), function->chunk.lines[instruction]);
	}
	return stacktrace;
}
}
