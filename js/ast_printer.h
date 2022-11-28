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
		std::cout << prefix << "Program\n";
		for (auto stmt : program)
			stmt->accept(this, "     ");
	}

	void visit(const BlockStmt *node, std::string const &prefix) const { 
		std::cout << prefix << "└──";
		std::cout << node->name() << "\n";

		int x = 0;
		for (auto stmt : node->stmts())
		{
			if (x++ == 0)
				stmt->accept(this, prefix + "    ");
			else
				stmt->accept(this, prefix + "│   ");
		}
	 }
	void visit(const VariableStmt *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }
	void visit(const EmptyStmt *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }
	void visit(const IfStmt *node, std::string const &prefix) const
	{
		std::cout << prefix;
		std::cout << "├──";
		std::cout << node->name() << "\n";

		std::cout << prefix << "│   " << "├──" << "condition: ";
		node->condition()->accept(this, "");

		std::cout << prefix << "│   " << "├──" << "then: ";
		//node->then()->accept(this, "");

		//node->condition()->accept(this, prefix + "│   " + "├──" + "condition: ");
		//node->then()->accept(this, prefix + "│   "  );
		//if (node->else_stmt())
		//	node->else_stmt()->accept(this,  prefix + "│   ");
	}

	void visit(const ReturnStmt *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }
	void visit(const ExpressionStmt *node, std::string const &prefix) const
	{
		std::cout << prefix << "└──";
		std::cout << node->name() << "\n";
		node->expr()->accept(this, prefix + "│   ");
	}

	void visit(const FunctionDecl *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }

	void visit(const UnaryExpr *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }
	void visit(const BinaryExpr *node, std::string const &prefix) const
	{
		std::cout << prefix;
		std::cout << "└──";
		std::cout << node->name() << "\n";

		node->lhs()->accept(this, prefix + "    " + "├──");
		std::cout << prefix << "    "
		          << "├──" << node->op().value() << "\n";
		node->rhs()->accept(this, prefix + "    " + +"└──");
	}
	void visit(const CallExpr *node, std::string const &prefix) const { std::cout << node->name() << "\n"; }
	void visit(const Literal *node, std::string const &prefix) const
	{
		std::cout << prefix;
		std::cout << node->value().as_number() << "\n";
	}
};
}
