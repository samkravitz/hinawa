#pragma once

#include "ast/ast.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/visitor.h"
#include <iostream>

namespace js
{
class AstPrinter : public PrintVisitor
{
public:
	AstPrinter() { }
	void print(std::vector<std::shared_ptr<Stmt>> program)
	{
		std::string prefix = "└──";
		std::cout << prefix << "Program";
		for (auto stmt : program)
			stmt->accept(this);
	}

	void visit(const BlockStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const VariableStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const EmptyStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const IfStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const ReturnStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const ExpressionStmt *node) const { std::cout << node->name() << "\n"; }
	void visit(const UnaryExpr *node) const { std::cout << node->name() << "\n"; }
	void visit(const BinaryExpr *node) const { std::cout << node->name() << "\n"; }
	void visit(const CallExpr *node) const { std::cout << node->name() << "\n"; }
	void visit(const Literal *node) const { std::cout << node->name() << "\n"; }
};
}
