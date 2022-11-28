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
		std::cout << "Script:\n";
		for (auto stmt : program)
			stmt->accept(this, 0);
	}

	void visit(const BlockStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";

		for (auto stmt : node->stmts())
			stmt->accept(this, indent + 1);
	}

	void visit(const VariableStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";
		print_indent(indent + 1);
		std::cout << node->identifier() << "\n";

		if (node->init())
			node->init()->accept(this, indent + 1);
	}

	void visit(const EmptyStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";
	}

	void visit(const IfStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";

		node->condition()->accept(this, indent + 1);
	}

	void visit(const ReturnStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";

		if (node->expr())
			node->expr()->accept(this, indent + 1);
	}

	void visit(const ExpressionStmt *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";
		node->expr()->accept(this, indent + 1);
	}

	void visit(const FunctionDecl *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << ": " << node->function_name() << "\n";
		node->block()->accept(this, indent + 1);
	}

	void visit(const UnaryExpr *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";
	}

	void visit(const BinaryExpr *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";

		node->lhs()->accept(this, indent + 1);
		print_indent(indent + 1);
		std::cout << node->op().value() << "\n";
		node->rhs()->accept(this, indent + 1);
	}

	void visit(const CallExpr *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->name() << "\n";
	}

	void visit(const Literal *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->value().as_number() << "\n";
	}

	void visit(const Variable *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->ident() << "\n";
	}

private:
	void print_indent(int indent) const
	{
		for (int i = 0; i < indent * 2; i++)
			std::cout << " ";
	}
};
}