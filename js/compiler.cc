#include "compiler.h"

#include <cassert>
#include <functional>
#include <iostream>

#include <fmt/format.h>

namespace js
{

Compiler::Compiler(const std::vector<Stmt*> &stmts) :
    stmts(stmts)
{ }

void Compiler::init_compiler(FunctionCompiler *compiler)
{
	compiler->enclosing = current;
	current = compiler;

	current->locals.push_back({ "", 0 });
}

void Compiler::end_compiler()
{
	if (current->function->chunk.code.empty())
		emit_constant(Value(nullptr));

	emit_byte(OP_RETURN);

	auto function = *current->function;
#ifdef DEBUG_PRINT_CODE
	const char *name = function.type == ANONYMOUS ? "anonymous" : function.name.c_str();
	function.chunk.disassemble(name);
#endif

	current = current->enclosing;
}

Function Compiler::compile()
{
	Function script("script", SCRIPT);
	FunctionCompiler compiler(current, &script);
	init_compiler(&compiler);

	for (auto stmt : stmts)
		stmt->accept(this);

	end_compiler();
	return script;
}

void Compiler::compile(const BlockStmt &stmt) { }

void Compiler::compile(const VariableStmt &stmt) { }

void Compiler::compile(const ExpressionStmt &stmt)
{
	stmt.expr->accept(this);
	emit_byte(OP_POP);
}

void Compiler::compile(const IfStmt &stmt) { }

void Compiler::compile(const ForStmt &stmt) { }

void Compiler::compile(const FunctionDecl &stmt) { }

void Compiler::compile(const EmptyStmt &stmt) { }
void Compiler::compile(const ReturnStmt &stmt) { }

void Compiler::compile(const UnaryExpr &expr) { }

void Compiler::compile(const UpdateExpr &expr) { }

void Compiler::compile(const BinaryExpr &expr)
{
	expr.lhs->accept(this);
	expr.rhs->accept(this);
	auto op = expr.op.type();

	switch (op)
	{
		case PLUS:
			emit_byte(OP_ADD);
			break;
		case MINUS:
			emit_byte(OP_SUBTRACT);
			break;
		case STAR:
			emit_byte(OP_MULTIPLY);
			break;
		case SLASH:
			emit_byte(OP_DIVIDE);
			break;
		case MOD:
			emit_byte(OP_MOD);
			break;
		case BANG_EQUAL:
			emit_bytes(OP_EQUAL, OP_NOT);
			break;
		case EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case EQUAL_EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case GREATER:
			emit_byte(OP_GREATER);
			break;
		case GREATER_EQUAL:
			emit_bytes(OP_LESS, OP_NOT);
			break;
		case LESS:
			emit_byte(OP_LESS);
			break;
		case LESS_EQUAL:
			emit_bytes(OP_GREATER, OP_NOT);
			break;
		case AND:
			emit_byte(OP_BITWISE_AND);
			break;
		case PIPE:
			emit_byte(OP_BITWISE_OR);
			break;
		case AND_AND:
			emit_byte(OP_LOGICAL_AND);
			break;
		case PIPE_PIPE:
			emit_byte(OP_LOGICAL_OR);
			break;
		default:
			fmt::print("Unknown binary op {}\n", expr.op.value());
	}
}

void Compiler::compile(const CallExpr &expr) { }

void Compiler::compile(const MemberExpr &expr) { }

void Compiler::compile(const Literal &expr)
{
	switch (expr.token.type())
	{
		case NUMBER:
		{
			auto d = std::stod(expr.token.value());
			emit_constant(Value(d));
			break;
		}
		case STRING:
		{
			auto str = expr.token.value();
			emit_constant(Value(new std::string(str.substr(1, str.size() - 2))));
			break;
		}
		case KEY_FALSE:
			emit_byte(OP_FALSE);
			break;
		case KEY_NULL:
			emit_byte(OP_NULL);
			break;
		case KEY_TRUE:
			emit_byte(OP_TRUE);
			break;
		case KEY_UNDEFINED:
			emit_byte(OP_UNDEFINED);
			break;
		default:
			assert(!"Unknown literal!");
	}
}

void Compiler::compile(const Variable &expr) { }

size_t Compiler::make_constant(Value value)
{
	auto constant = current_function().chunk.add_constant(value);
	if (constant > 0xff)
	{
		std::cerr << "Too many constants in this chunk\n";
		return 0;
	}

	return constant;
}

void Compiler::emit_byte(u8 byte)
{
	current_function().chunk.write(byte, 0);
}

void Compiler::emit_bytes(u8 a, u8 b)
{
	emit_byte(a);
	emit_byte(b);
}

void Compiler::emit_constant(Value value)
{
	auto constant = make_constant(value);
	emit_bytes(OP_CONSTANT, constant);
}
}
