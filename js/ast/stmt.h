#pragma once

#include "ast.h"
#include "expr.h"

namespace js
{
class Stmt : public AstNode
{
public:
	const char *name() const = 0;
};

class BlockStmt : public Stmt
{
public:
	const char *name() const { return "BlockStmt"; }
};

class VariableStmt : public Stmt
{
public:
	const char *name() const { return "VariableStmt"; }
};

class EmptyStmt : public Stmt
{
public:
	const char *name() const { return "EmptyStmt"; }
};

class ExpressionStmt : public Stmt
{
public:
	ExpressionStmt(std::shared_ptr<Expr> expr) :
	    m_expr(expr)
	{ }

	const char *name() const { return "ExpressionStmt"; }
	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		m_expr->print(prefix + (is_left ? "│   " : "    "), true);
	}

private:
	std::shared_ptr<Expr> m_expr;
};

class IfStmt : public Stmt
{
public:
	const char *name() const { return "IfStmt"; }
};

class ContinueStmt : public Stmt
{
public:
	const char *name() const { return "ContinueStmt"; }
};

class BreakStmt : public Stmt
{
public:
	const char *name() const { return "BreakStmt"; }
};

class ReturnStmt : public Stmt
{
public:
	const char *name() const { return "ReturnStmt"; }
};

class TryStmt : public Stmt
{
public:
	const char *name() const { return "TryStmt"; }
};

class FunctionDeclaration : public AstNode
{
public:
	const char *name() const { return "FunctionDeclaration"; }
};

class VariableDeclaration : public AstNode
{
public:
	const char *name() const { return "VariableDeclaration"; }
};

class ExpressionStatement : public AstNode
{
public:
	const char *name() const { return "ExpressionStatement"; }
};

class BlockStatement : public AstNode
{
public:
	const char *name() const { return "BlockStatement"; }
};
}
