#include "compiler.h"

#include <cassert>
#include <functional>
#include <iostream>

#include <fmt/format.h>

namespace js
{

Compiler::Compiler(const std::vector<Stmt *> &stmts) :
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

void Compiler::compile(const BlockStmt &stmt)
{
	begin_scope();

	for (auto s : stmt.stmts)
		s->accept(this);

	end_scope();
}

void Compiler::compile(const VarDecl &stmt)
{
	u8 global = identifier_constant(stmt.identifier);

	if (stmt.init)
		stmt.init->accept(this);
	else
		emit_byte(OP_UNDEFINED);
	
	define_variable(global);
}

void Compiler::compile(const ExpressionStmt &stmt)
{
	stmt.expr->accept(this);
	emit_byte(OP_POP);
}

void Compiler::compile(const IfStmt &stmt) { }

void Compiler::compile(const ForStmt &stmt)
{
	begin_scope();

	if (stmt.initialization)
		stmt.initialization->accept(this);

	int loop_start = current->function->chunk.size();
	int exit_jump = -1;

	if (stmt.condition)
	{
		stmt.condition->accept(this);
		exit_jump = emit_jump(OP_JUMP_IF_FALSE);
		emit_byte(OP_POP);
	}

	stmt.statement->accept(this);
	if (stmt.afterthought)
	{
		stmt.afterthought->accept(this);
		emit_byte(OP_POP);
	}
	
	emit_loop(loop_start);
	
	if (exit_jump != -1)
	{
		patch_jump(exit_jump);
		emit_byte(OP_POP);
	}

	end_scope();
}

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

void Compiler::compile(const AssignmentExpr &expr)
{
	expr.rhs->accept(this);
	expr.lhs->accept(this);
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

void Compiler::compile(const Variable &expr)
{
	auto value = make_constant(Value(new std::string{expr.ident}));
	if (expr.is_assign)
		emit_bytes(OP_SET_GLOBAL, value);
	else
	 	emit_bytes(OP_GET_GLOBAL, value);
}

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

size_t Compiler::emit_jump(Opcode op)
{
	emit_byte(op);
	emit_bytes(0xff, 0xff);
	return current_function().chunk.size() - 2;
}

void Compiler::patch_jump(size_t offset)
{
	auto jump = current_function().chunk.size() - offset - 2;
	if (jump > 0xffff)
		std::cerr << "Jump is out of bounds\n";

	current_function().chunk.code[offset] = (jump >> 8) & 0xff;
	current_function().chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(size_t loop_start)
{
	emit_byte(OP_LOOP);
	auto offset = current_function().chunk.size() - loop_start + 2;
	if (offset > 0xffff)
		std::cerr << "Loop offset is out of bounds\n";

	emit_byte((offset >> 8) & 0xff);
	emit_byte(offset & 0xff);
}

void Compiler::define_variable(u8 global)
{
	emit_bytes(OP_DEFINE_GLOBAL, global);
}

u8 Compiler::identifier_constant(const std::string &name)
{
	return make_constant(Value(new std::string{name}));
}

void Compiler::begin_scope()
{
	current->scope_depth += 1;
}

void Compiler::end_scope()
{
	current->scope_depth -= 1;
	while (!current->locals.empty() && (current->locals.back().depth > current->scope_depth))
	{
		emit_byte(OP_POP);
		current->locals.pop_back();
	}
}

/**
 * @brief returns true if currently compiling for the global scope
*/
bool Compiler::is_global()
{
	return current->scope_depth == 0;
}
}
