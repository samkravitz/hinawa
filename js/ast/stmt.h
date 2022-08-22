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
	BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) :
	    m_stmts(stmts)
	{ }

	const char *name() const { return "BlockStmt"; }

	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		int x = 0;
		for (auto stmt : m_stmts)
		{
			if (x++ == 0)
				stmt->print(prefix + (is_left ? "│   " : "    "), true);
			else
				stmt->print(prefix + (is_left ? "│   " : "    "), false);
		}
	}

private:
	std::vector<std::shared_ptr<Stmt>> m_stmts;
};

class VariableStmt : public Stmt
{
public:
	VariableStmt(std::string identifier, std::shared_ptr<Expr> initializer) :
	    m_identifier(identifier),
	    m_init(initializer)
	{ }
	const char *name() const { return "VariableStmt"; }

	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		std::cout << prefix << "│   " << (m_init ? "├──" : "└──") << "name: " << m_identifier << "\n";
		if (m_init)
			m_init->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	std::string m_identifier;
	std::shared_ptr<Expr> m_init;
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

		m_expr->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	std::shared_ptr<Expr> m_expr;
};

class IfStmt : public Stmt
{
public:
	IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then, std::shared_ptr<Stmt> else_stmt) :
	    m_condition(condition),
	    m_then(then),
	    m_else(else_stmt)
	{ }

	const char *name() const { return "IfStmt"; }

	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		m_condition->print(prefix + (is_left ? "│   " : "    "), true);
		m_then->print(prefix + (is_left ? "│   " : "    "), m_else != nullptr);

		if (m_else)
			m_else->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	std::shared_ptr<Expr> m_condition;
	std::shared_ptr<Stmt> m_then;
	std::shared_ptr<Stmt> m_else;
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
	ReturnStmt(std::shared_ptr<Expr> expr) :
	    m_expr(expr)
	{ }

	const char *name() const { return "ReturnStmt"; }

	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		if (m_expr)
			m_expr->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	std::shared_ptr<Expr> m_expr;
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
}
