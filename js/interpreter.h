#pragma once

#include <memory>

#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/visitor.h"

namespace js
{
class Interpreter : public StmtVisitor, public ExprVisitor
{
public:
	Interpreter() = default;

	void run(std::shared_ptr<Program>);

	// statement visitors
	void visit(const Program *) const;
	void visit(const BlockStmt *) const;
	void visit(const VariableStmt *) const;
	void visit(const EmptyStmt *) const;
	void visit(const IfStmt *) const;
	void visit(const ReturnStmt *) const;
	void visit(const ExpressionStmt *) const;

	// expression visitors

	Value visit(const UnaryExpr *) const;
	Value visit(const BinaryExpr *) const;
	Value visit(const CallExpr *) const;
	Value visit(const Literal *) const;

private:
	void execute(const std::shared_ptr<Stmt>);
	Value evaluate(const std::shared_ptr<Expr>) const;
};
}
