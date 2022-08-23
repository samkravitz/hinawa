#include "interpreter.h"

namespace js
{
void Interpreter::run(std::shared_ptr<Program> program)
{
	for (auto stmt : program->stmts())
		execute(stmt);
}

void Interpreter::execute(const std::shared_ptr<Stmt> stmt)
{
	stmt->accept(this);
}

Value Interpreter::evaluate(const std::shared_ptr<Expr> expr) const
{
	return expr->accept(this);
}

void Interpreter::visit(const Program *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const BlockStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const VariableStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const EmptyStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const IfStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const ReturnStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
}

void Interpreter::visit(const ExpressionStmt *stmt) const
{
	std::cout << stmt->name() << "\n";
	std::cout << evaluate(stmt->expr()).as_number() << "\n";
}

// expression visitors

Value Interpreter::visit(const UnaryExpr *expr) const
{
	auto rhs = expr->rhs();
	auto op = expr->op();

	if (op.type() == MINUS)
		return Value(-evaluate(rhs).as_number());

	return Value(0.0);
}

Value Interpreter::visit(const BinaryExpr *expr) const
{
	auto rhs = evaluate(expr->lhs());
	auto op = expr->op();
	auto lhs = evaluate(expr->rhs());

	switch (op.type())
	{
		case MINUS: return Value(lhs.as_number() - rhs.as_number());
		case PLUS: return Value(lhs.as_number() + rhs.as_number());
		case SLASH: return Value(lhs.as_number() * rhs.as_number());
		case STAR: return Value(lhs.as_number() / rhs.as_number());
		default:
			;
	}

	return Value(0.0);
}

Value Interpreter::visit(const CallExpr *expr) const
{
	std::cout << expr->name() << "\n";
	return Value(Value::Type::Undefined);
}

Value Interpreter::visit(const Literal *expr) const
{
	return expr->value();
}
}
