#include "compiler.h"

#include <cassert>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>

#include "heap.h"
#include "object_string.h"
#include "opcode.h"

namespace js
{
static constexpr int RESOLVED_GLOBAL = -1;

Compiler::Compiler(const std::vector<std::shared_ptr<Stmt>> &stmts) :
    stmts(stmts)
{ }

void Compiler::init_compiler(FunctionCompiler *compiler)
{
	compiler->enclosing = current;
	current = compiler;

	current->locals.push_back({"this", -1});
}

void Compiler::end_compiler()
{
	emit_bytes(OP_UNDEFINED, OP_RETURN);

	auto function = *current->function;
#ifdef DEBUG_PRINT_CODE
	const char *name = function.type == ANONYMOUS ? "anonymous" : function.name->string().c_str();
	function.chunk.disassemble(name);
#endif

	current = current->enclosing;
}

Function *Compiler::compile(const std::vector<std::shared_ptr<Stmt>> &stmts)
{
	Compiler c(stmts);
	return c.compile_impl();
}

Function *Compiler::compile_impl()
{
	auto *script = heap().allocate<Function>("script", SCRIPT);
	FunctionCompiler compiler(current, script);
	init_compiler(&compiler);

	for (auto stmt : stmts)
		stmt->accept(this);

	end_compiler();
	return script;
}

void Compiler::compile(const BlockStmt &stmt)
{
	current_line = stmt.line;
	begin_scope();
	for (auto s : stmt.stmts)
		s->accept(this);
	end_scope();
}

void Compiler::compile(const VarDecl &stmt)
{
	current_line = stmt.line;
	for (const auto &declarator : stmt.declorators)
	{
		auto global = parse_variable(declarator.identifier);
		if (declarator.init)
			declarator.init->accept(this);
		else
			emit_byte(OP_UNDEFINED);

		define_variable(global);
	}
}

void Compiler::compile(const ExpressionStmt &stmt)
{
	current_line = stmt.line;
	stmt.expr->accept(this);
	emit_byte(OP_POP);
}

void Compiler::compile(const IfStmt &stmt)
{
	current_line = stmt.line;
	stmt.test->accept(this);
	auto then_offset = emit_jump(OP_JUMP_IF_FALSE);
	emit_byte(OP_POP);
	stmt.consequence->accept(this);

	auto else_offset = emit_jump(OP_JUMP);
	emit_byte(OP_POP);
	patch_jump(then_offset);

	if (stmt.alternate)
		stmt.alternate->accept(this);

	patch_jump(else_offset);
}

void Compiler::compile(const ForStmt &stmt)
{
	current_line = stmt.line;
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

void Compiler::compile(const FunctionDecl &stmt)
{
	current_line = stmt.line;
	auto global = parse_variable(stmt.function_name);
	auto *function = heap().allocate<Function>(stmt.function_name);
	function->arity = stmt.args.size();
	FunctionCompiler compiler(current, function);
	init_compiler(&compiler);
	begin_scope();

	for (const auto &arg : stmt.args)
	{
		auto constant = parse_variable(arg);
		define_variable(constant);
	}

	stmt.block->accept(this);
	end_compiler();
	function->upvalue_count = compiler.upvalue_count();
	emit_bytes(OP_CLOSURE, make_constant(Value(heap().allocate<Function>(*function))));

	for (const auto &upvalue : compiler.upvalues)
	{
		emit_byte(upvalue.is_local ? 1 : 0);
		emit_byte(upvalue.index);
	}

	define_variable(global);
}

void Compiler::compile(const EmptyStmt &stmt) { }

void Compiler::compile(const ReturnStmt &stmt)
{
	current_line = stmt.line;
	if (stmt.expr)
		stmt.expr->accept(this);
	else
		emit_byte(OP_UNDEFINED);

	emit_byte(OP_RETURN);
}

void Compiler::compile(const ThrowStmt &stmt)
{
	current_line = stmt.line;
	stmt.expr->accept(this);
	emit_byte(OP_THROW);
}

void Compiler::compile(const TryStmt &stmt)
{
	current_line = stmt.line;
	int catch_jump = emit_jump(OP_PUSH_EXCEPTION);
	stmt.block->accept(this);
	int finally_jump = emit_jump(OP_JUMP);
	patch_jump(catch_jump);

	if (stmt.handler)
	{
		if (stmt.catch_param)
		{
			begin_scope();
			auto constant = parse_variable(*stmt.catch_param);
			define_variable(constant);
		}
		else
			emit_byte(OP_POP);

		stmt.handler->accept(this);
		end_scope();
	}

	emit_byte(OP_POP_EXCEPTION);
	patch_jump(finally_jump);

	if (stmt.finalizer)
	{
		stmt.finalizer->accept(this);
	}
}

void Compiler::compile(const UnaryExpr &expr)
{
	current_line = expr.line;
	expr.rhs->accept(this);
	auto op = expr.op;

	switch (op.type())
	{
		case KEY_TYPEOF:
			emit_byte(OP_TYPEOF);
			break;
		case MINUS:
			emit_byte(OP_NEGATE);
			break;
		case BANG:
			emit_byte(OP_NOT);
			break;
		default:
			fmt::print(stderr, "Unknown unary op {}\n", op.value());
	}
}

void Compiler::compile(const UpdateExpr &expr)
{
	current_line = expr.line;
	expr.operand->accept(this);

	Opcode inc_or_dec{};
	switch (expr.op.type())
	{
		case PLUS_PLUS:
			inc_or_dec = OP_INCREMENT;
			break;
		case MINUS_MINUS:
			inc_or_dec = OP_DECREMENT;
			break;
		default:
			assert(!fmt::format("Unknown update expr {}", expr.op.value()).c_str());
	}

	emit_byte(inc_or_dec);
	assignment_target(*expr.operand);
	emit_byte(OP_POP);

	if (expr.prefix)
	{
		emit_byte(OP_POP);
		expr.operand->accept(this);
	}
}

void Compiler::compile(const BinaryExpr &expr)
{
	current_line = expr.line;
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
			emit_byte(OP_STRICT_EQUAL);
			break;
		case BANG_EQUAL_EQUAL:
			emit_bytes(OP_STRICT_EQUAL, OP_NOT);
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
		case KEY_INSTANCEOF:
			emit_byte(OP_INSTANCEOF);
			break;
		default:
			fmt::print("Unknown binary op {}\n", expr.op.value());
	}
}

void Compiler::compile(const AssignmentExpr &expr)
{
	current_line = expr.line;
	std::string identifier;
	int value{};
	if (expr.lhs->is_variable())
	{
		Opcode set_op{};
		auto &var = static_cast<Variable &>(*expr.lhs);
		identifier = var.ident;
		value = resolve_local(current, identifier);

		if (value != RESOLVED_GLOBAL)
		{
			set_op = OP_SET_LOCAL;
		}

		else if ((value = resolve_upvalue(current, identifier)) != -1)
		{
			set_op = OP_SET_UPVALUE;
		}

		else
		{
			set_op = OP_SET_GLOBAL;
			value = make_constant(Value(heap().allocate_string(identifier)));
		}

		expr.rhs->accept(this);

		if (expr.op.value() == "+=")
		{
			expr.lhs->accept(this);
			emit_byte(OP_ADD);
		}

		emit_bytes(set_op, value);
		return;
	}

	else if (expr.lhs->is_member_expr())
	{
		auto &member = static_cast<const MemberExpr &>(*expr.lhs);
		member.object->accept(this);
		if (member.is_dot)
		{
			assert(member.property->is_variable());
			expr.rhs->accept(this);
			auto &variable = static_cast<Variable &>(*member.property);
			auto identifier = variable.ident;
			auto constant = make_constant(Value(heap().allocate_string(identifier)));
			emit_bytes(OP_SET_PROPERTY, constant);
		}

		else
		{
			member.property->accept(this);
			expr.rhs->accept(this);
			emit_byte(OP_SET_SUBSCRIPT);
		}
	}

	else
	{
		assert(!"AssignmentExpr lhs is not variable or member expression\n");
	}
}

void Compiler::compile(const CallExpr &expr)
{
	current_line = expr.line;
	expr.callee->accept(this);
	for (const auto &ex : expr.args)
		ex->accept(this);

	emit_bytes(OP_CALL, expr.args.size());
}

void Compiler::compile(const MemberExpr &expr)
{
	current_line = expr.line;
	expr.object->accept(this);

	if (expr.is_dot)
	{
		assert(expr.property->is_variable());
		auto &variable = static_cast<Variable &>(*expr.property);
		auto identifier = variable.ident;
		auto constant = make_constant(Value(heap().allocate_string(identifier)));
		emit_bytes(OP_GET_PROPERTY, constant);
	}

	else
	{
		expr.property->accept(this);
		emit_byte(OP_GET_SUBSCRIPT);
	}
}

void Compiler::compile(const Literal &expr)
{
	current_line = expr.line;
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
			auto quotes_removed = str.substr(1, str.size() - 2);
			emit_constant(Value(heap().allocate_string(quotes_removed)));
			break;
		}
		case IDENTIFIER:
		{
			assert(false);
			const auto identifier = expr.token.value();
			Opcode get_op;
			int value = resolve_local(current, identifier);

			if (value != RESOLVED_GLOBAL)
			{
				get_op = OP_GET_LOCAL;
			}

			else if ((value = resolve_upvalue(current, identifier)) != -1)
			{
				get_op = OP_GET_UPVALUE;
			}

			else
			{
				get_op = OP_GET_GLOBAL;
				value = make_constant(Value(heap().allocate_string(identifier)));
			}

			emit_bytes(get_op, value);
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
	current_line = expr.line;
	auto identifier = expr.ident;

	Opcode get_op;
	int value = resolve_local(current, identifier);

	if (value != RESOLVED_GLOBAL)
	{
		get_op = OP_GET_LOCAL;
	}

	else if ((value = resolve_upvalue(current, identifier)) != -1)
	{
		get_op = OP_GET_UPVALUE;
	}

	else
	{
		get_op = OP_GET_GLOBAL;
		value = make_constant(Value(heap().allocate_string(identifier)));
	}

	emit_bytes(get_op, value);
}

void Compiler::compile(const ObjectExpr &expr)
{
	current_line = expr.line;
	for (const auto &property : expr.properties)
		property.second->accept(this);

	emit_bytes(OP_NEW_OBJECT, expr.properties.size());
	for (const auto &property : expr.properties)
		emit_byte(identifier_constant(property.first));
}

void Compiler::compile(const FunctionExpr &expr)
{
	current_line = expr.line;
	auto function = heap().allocate<Function>(ANONYMOUS);
	function->arity = expr.args.size();
	FunctionCompiler compiler(current, function);
	init_compiler(&compiler);
	begin_scope();

	for (const auto &arg : expr.args)
	{
		auto constant = parse_variable(arg);
		define_variable(constant);
	}

	expr.body->accept(this);
	end_compiler();
	function->upvalue_count = compiler.upvalue_count();
	emit_bytes(OP_CLOSURE, make_constant(Value(heap().allocate<Function>(*function))));

	for (const auto &upvalue : compiler.upvalues)
	{
		emit_byte(upvalue.is_local ? 1 : 0);
		emit_byte(upvalue.index);
	}
}

void Compiler::compile(const NewExpr &expr)
{
	current_line = expr.line;
	expr.callee->accept(this);

	for (const auto &ex : expr.args)
		ex->accept(this);

	emit_bytes(OP_CALL_CONSTRUCTOR, expr.args.size());
}

void Compiler::compile(const ArrayExpr &expr)
{
	current_line = expr.line;
	for (const auto &element : expr.elements)
		element->accept(this);

	emit_bytes(OP_NEW_ARRAY, expr.elements.size());
}

void Compiler::assignment_target(const Expr &expr)
{
	std::string identifier;
	int value{};
	if (expr.is_variable())
	{
		Opcode set_op{};
		auto &var = static_cast<const Variable &>(expr);
		identifier = var.ident;
		value = resolve_local(current, identifier);

		if (value != RESOLVED_GLOBAL)
		{
			set_op = OP_SET_LOCAL;
		}

		else if ((value = resolve_upvalue(current, identifier)) != -1)
		{
			set_op = OP_SET_UPVALUE;
		}

		else
		{
			set_op = OP_SET_GLOBAL;
			value = make_constant(Value(heap().allocate_string(identifier)));
		}

		emit_bytes(set_op, value);
		return;
	}

	else if (expr.is_member_expr())
	{
		auto &member = static_cast<const MemberExpr &>(expr);
		member.object->accept(this);
		if (member.is_dot)
		{
			assert(member.property->is_variable());
			auto &variable = static_cast<Variable &>(*member.property);
			auto identifier = variable.ident;
			auto constant = make_constant(Value(heap().allocate_string(identifier)));
			emit_bytes(OP_SET_PROPERTY, constant);
		}

		else
		{
			member.property->accept(this);
			emit_byte(OP_GET_SUBSCRIPT);
		}
	}

	else
	{
		assert(!"Expr is not a valid assignment target\n");
	}
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
	current_function().chunk.write(byte, current_line);
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

u8 Compiler::parse_variable(const std::string &identifier)
{
	if (!is_global())
	{
		declare_local(identifier);
		return 0;
	}

	return identifier_constant(identifier);
}

void Compiler::define_variable(u8 global)
{
	if (!is_global())
	{
		mark_initialized();
		return;
	}

	emit_bytes(OP_DEFINE_GLOBAL, global);
}

u8 Compiler::identifier_constant(const std::string &name)
{
	return make_constant(Value(heap().allocate_string(name)));
}

void Compiler::mark_initialized()
{
	if (is_global())
		return;

	current->locals.back().depth = current->scope_depth;
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

	current->locals.push_back({name, current->scope_depth});
}

int Compiler::resolve_local(FunctionCompiler *compiler, const std::string &name)
{
	auto local_count = compiler->local_count();
	// Using C++20 ranges: iterate backwards
	// for (auto &local : current->locals | std::ranges::views::reverse)
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = compiler->locals[i];
		if (name == local.name)
			return i;
	}
	return RESOLVED_GLOBAL;
}

int Compiler::resolve_upvalue(FunctionCompiler *compiler, const std::string &name)
{
	if (!compiler->enclosing)
		return -1;

	int local = resolve_local(compiler->enclosing, name);
	if (local != -1)
		return add_upvalue(compiler, local, true);

	int upvalue = resolve_upvalue(compiler->enclosing, name);
	if (upvalue != -1)
		return add_upvalue(compiler, upvalue, false);

	return -1;
}

int Compiler::add_upvalue(FunctionCompiler *compiler, u8 index, bool is_local)
{
	for (unsigned i = 0; i < compiler->upvalues.size(); i++)
	{
		const auto &upvalue = compiler->upvalues[i];
		if (upvalue.index == index && upvalue.is_local)
			return i;
	}

	compiler->upvalues.push_back({index, is_local});
	return compiler->upvalues.size() - 1;
}

/**
 * @brief returns true if currently compiling for the global scope
*/
bool Compiler::is_global() const
{
	return current->scope_depth == 0;
}
}
