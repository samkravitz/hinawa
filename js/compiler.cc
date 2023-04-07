#include "compiler.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <ranges>

#include <fmt/format.h>

namespace js
{
static constexpr int RESOLVED_GLOBAL = -1;

Compiler::Compiler(const std::vector<Stmt *> &stmts) :
    stmts(stmts)
{ }

void Compiler::init_compiler(FunctionCompiler *compiler)
{
	compiler->enclosing = current;
	current = compiler;

	current->locals.push_back({"", 0});
	for (size_t i = 0; i <= 0xff; i++)
		available_regs.push(i);
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

std::optional<size_t> Compiler::compile(const BlockStmt &stmt)
{
	begin_scope();

	for (auto s : stmt.stmts)
		s->accept(this);

	end_scope();
	return {};
}

std::optional<size_t> Compiler::compile(const VarDecl &stmt)
{
	if (is_global())
	{
		u8 global = identifier_constant(stmt.identifier);

		if (stmt.init)
			stmt.init->accept(this);
		else
			emit_byte(OP_UNDEFINED);

		define_variable(global);
		return {};
	}

	else
	{
		declare_local(stmt.identifier);
		auto &local = current->locals.back();

		if (stmt.init)
		{
			auto reg = stmt.init->accept(this);
			assert(reg);
			emit_byte(OP_MOV);
			emit_bytes(local.reg, *reg);
			free_reg(*reg);
		}
		else
			local.reg = allocate_reg();
	}

	return {};
}

std::optional<size_t> Compiler::compile(const ExpressionStmt &stmt)
{
	auto reg = stmt.expr->accept(this);
	if (reg)
		free_reg(*reg);
	return reg;
}

std::optional<size_t> Compiler::compile(const IfStmt &stmt)
{
	return {};
}

std::optional<size_t> Compiler::compile(const ForStmt &stmt)
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
	return {};
}

std::optional<size_t> Compiler::compile(const FunctionDecl &stmt)
{
	return {};
}

std::optional<size_t> Compiler::compile(const EmptyStmt &stmt)
{
	return {};
}
std::optional<size_t> Compiler::compile(const ReturnStmt &stmt)
{
	return {};
}

std::optional<size_t> Compiler::compile(const UnaryExpr &expr)
{
	return {};
}

std::optional<size_t> Compiler::compile(const UpdateExpr &expr)
{
	return {};
}

std::optional<size_t> Compiler::compile(const BinaryExpr &expr)
{
	auto lhs = expr.lhs->accept(this);
	auto rhs = expr.rhs->accept(this);
	assert(lhs);
	assert(rhs);
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
	auto reg = allocate_reg();
	emit_byte(reg);
	emit_bytes(*lhs, *rhs);
	free_reg(*lhs);
	free_reg(*rhs);
	return reg;
}

std::optional<size_t> Compiler::compile(const AssignmentExpr &expr)
{
	auto rhs = expr.rhs->accept(this);
	assert(rhs);
	if (expr.lhs->is_variable())
	{
		auto &var = static_cast<Variable &>(*expr.lhs);
		auto reg = resolve_local(var.ident);

		if (reg == RESOLVED_GLOBAL)
		{
			auto k = make_constant(Value(new std::string{var.ident}));
			emit_byte(OP_SET_GLOBAL);
			emit_bytes(*rhs, k);
		}
		else
		{
			emit_byte(OP_MOV);
			emit_bytes(reg, *rhs);
		}
	}
	else
	{
		std::cout << "AssignmentExpr lhs is not variable\n";
	}
	free_reg(*rhs);
	return rhs;
}

std::optional<size_t> Compiler::compile(const CallExpr &expr)
{
	return {};
}

std::optional<size_t> Compiler::compile(const MemberExpr &expr)
{
	return {};
}

std::optional<size_t> Compiler::compile(const Literal &expr)
{
	switch (expr.token.type())
	{
		case NUMBER:
		{
			auto d = std::stod(expr.token.value());
			auto constant = make_constant(Value(d));
			auto reg = allocate_reg();
			emit_byte(OP_LOADK);
			emit_bytes(reg, constant);
			return reg;
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

	return {};
}

std::optional<size_t> Compiler::compile(const Variable &expr)
{
	auto value = make_constant(Value(new std::string{expr.ident}));
	if (expr.is_assign)
		emit_bytes(OP_SET_GLOBAL, value);
	else
		emit_bytes(OP_GET_GLOBAL, value);

	return {};
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
		auto &local = current->locals.back();
		free_reg(local.reg);
		current->locals.pop_back();
	}
}

void Compiler::declare_local(const std::string &name)
{
	auto local_count = current->local_count();
	// Using C++20 ranges: iterate backwards
	// for (auto &local : current->locals | std::ranges::views::reverse)
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = current->locals[i];
		if (local.depth != -1 && local.depth < current->scope_depth)
			break;

		if (name == local.name)
			fmt::print(stderr, "Already a variable with this name in this scope.\n");
	}

	current->locals.push_back({name, current->scope_depth, allocate_reg()});
}

int Compiler::resolve_local(const std::string &name)
{
	auto local_count = current->local_count();
	// Using C++20 ranges: iterate backwards
	// for (auto &local : current->locals | std::ranges::views::reverse)
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = current->locals[i];
		if (name == local.name)
			return local.reg;
	}
	return RESOLVED_GLOBAL;
}

/**
 * @brief returns true if currently compiling for the global scope
*/
bool Compiler::is_global() const
{
	return current->scope_depth == 0;
}

size_t Compiler::allocate_reg()
{
	if (available_regs.empty())
	{
		fmt::print(stderr, "Error: All registers in use\n");
		throw;
	}

	auto reg = available_regs.top();
	available_regs.pop();
	return reg;
}

void Compiler::free_reg(size_t reg)
{
	available_regs.push(reg);
}
}
