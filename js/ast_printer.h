#pragma once

#include "ast/ast.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/visitor.h"
#include <fmt/format.h>
#include <iostream>
#include <memory>

namespace js
{
void print_indent(int indent)
{
	for (int i = 0; i < indent * 2; i++)
		fmt::print(" ");
}

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
		node->print_header(indent);

		for (auto stmt : node->stmts)
			stmt->accept(this, indent + 1);
	}

	void visit(const VarDecl *node, int indent) const
	{
		node->print_header(indent);

		for (const auto &declarator : node->declorators)
		{
			print_indent(indent + 1);
			fmt::print("{}\n", declarator.identifier);

			if (declarator.init)
				declarator.init->accept(this, indent + 1);
		}
	}

	void visit(const EmptyStmt *node, int indent) const { node->print_header(indent); }

	void visit(const IfStmt *node, int indent) const
	{
		node->print_header(indent);
		node->test->accept(this, indent + 1);
	}

	void visit(const ReturnStmt *node, int indent) const
	{
		node->print_header(indent);

		if (node->expr)
			node->expr->accept(this, indent + 1);
	}

	void visit(const ExpressionStmt *node, int indent) const
	{
		node->print_header(indent);
		node->expr->accept(this, indent + 1);
	}

	void visit(const FunctionDecl *node, int indent) const
	{
		node->print_header(indent);
		node->block->accept(this, indent + 1);
	}

	void visit(const ForStmt *node, int indent) const
	{
		node->print_header(indent);

		print_indent(indent);
		std::cout << "initialization: ";
		if (!node->initialization)
			std::cout << "<null>\n";
		else
		{
			std::cout << '\n';
			node->initialization->accept(this, indent + 1);
		}

		print_indent(indent);
		std::cout << "condition: ";
		if (!node->condition)
			std::cout << "<null>\n";
		else
		{
			std::cout << '\n';
			node->condition->accept(this, indent + 1);
		}

		print_indent(indent);
		std::cout << "statement: \n";
		node->statement->accept(this, indent + 1);
		std::cout << '\n';

		print_indent(indent);
		std::cout << "afterthought: ";
		if (!node->afterthought)
			std::cout << "<null>\n";
		else
		{
			std::cout << '\n';
			node->afterthought->accept(this, indent + 1);
		}
	}

	void visit(const WhileStmt *node, int indent) const
	{
		node->print_header(indent);

		print_indent(indent);
		fmt::print("condition: \n");
		node->condition->accept(this, indent + 1);

		print_indent(indent);
		fmt::print("statement: \n");
		node->statement->accept(this, indent + 1);
	}

	void visit(const ContinueStmt *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent + 1);
		fmt::print("ContinueStmt (label: {})\n", node->label);
	}

	void visit(const BreakStmt *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent + 1);
		fmt::print("BreakStmt (label: {})\n", node->label);
	}

	void visit(const ThrowStmt *node, int indent) const
	{
		node->print_header(indent);
		node->expr->accept(this, indent + 1);
	}

	void visit(const TryStmt *node, int indent) const { node->print_header(indent); }

	void visit(const UnaryExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent + 1);
		fmt::print("{}\n", node->op.value());
		node->rhs->accept(this, indent + 1);
	}

	void visit(const UpdateExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent);
		std::cout << "op: " << node->op.value() << '\n';
		print_indent(indent);
		std::cout << "operand: \n";
		node->operand->accept(this, indent + 1);
		print_indent(indent);
		std::cout << "prefix: " << node->prefix << '\n';
	}

	void visit(const BinaryExpr *node, int indent) const
	{
		node->print_header(indent);

		node->lhs->accept(this, indent + 1);
		print_indent(indent + 1);
		std::cout << node->op.value() << "\n";
		node->rhs->accept(this, indent + 1);
	}

	void visit(const LogicalExpr *node, int indent) const
	{
		node->print_header(indent);

		node->lhs->accept(this, indent + 1);
		print_indent(indent + 1);
		std::cout << node->op.value() << "\n";
		node->rhs->accept(this, indent + 1);
	}

	void visit(const AssignmentExpr *node, int indent) const
	{
		node->print_header(indent);

		node->lhs->accept(this, indent + 1);
		print_indent(indent);
		fmt::print("op: {}\n", node->op.value());
		print_indent(indent);
		node->rhs->accept(this, indent);
	}

	void visit(const CallExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent);
		std::cout << "callee: \n";
		node->callee->accept(this, indent + 1);

		if (!node->args.empty())
		{
			print_indent(indent);
			std::cout << "args: \n";
			for (const auto &arg : node->args)
			{
				arg->accept(this, indent + 1);
			}
		}
	}

	void visit(const MemberExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent);
		std::cout << "object: \n";
		node->object->accept(this, indent + 1);

		print_indent(indent);
		fmt::print("property:\n");
		print_indent(indent);
		node->property->accept(this, indent);
	}

	void visit(const Literal *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->token.value() << "\n";
	}

	void visit(const Variable *node, int indent) const
	{
		print_indent(indent);
		std::cout << node->ident << "\n";
	}

	void visit(const ObjectExpr *node, int indent) const { node->print_header(indent); }

	void visit(const FunctionExpr *node, int indent) const
	{
		node->print_header(indent);
		node->body->accept(this, indent + 1);
	}

	void visit(const NewExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent);
		fmt::print("callee:\n");
		node->callee->accept(this, indent + 1);

		if (!node->args.empty())
		{
			print_indent(indent);
			fmt::print("args:\n");

			for (const auto &arg : node->args)
				arg->accept(this, indent + 1);
		}
	}

	void visit(const ArrayExpr *node, int indent) const
	{
		node->print_header(indent);
		print_indent(indent);
		if (!node->elements.empty())
		{
			print_indent(indent);
			fmt::print("elements:\n");

			for (const auto &arg : node->elements)
				arg->accept(this, indent + 1);
		}
	}

	void visit(const TernaryExpr *node, int indent) const
	{
		node->print_header(indent);

		print_indent(indent);
		fmt::print("condition: \n");
		print_indent(indent);
		node->condition->accept(this, indent + 1);

		print_indent(indent);
		fmt::print("if true: \n");
		print_indent(indent);
		node->if_true->accept(this, indent + 1);

		print_indent(indent);
		fmt::print("if false: \n");
		print_indent(indent);
		node->if_false->accept(this, indent + 1);
	}
};
}
