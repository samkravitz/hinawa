#pragma once

#include "../token.h"
#include "../value.h"
#include "ast.h"

namespace js
{
class Expr : public AstNode
{
public:
	virtual const char *name() const = 0;
};

class UnaryExpr : public Expr
{
public:
	UnaryExpr(Token op, std::shared_ptr<Expr> rhs) :
	    m_op(op),
	    m_rhs(rhs)
	{ }

	const char *name() const { return "UnaryExpr"; }
	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		std::cout << prefix << (is_left ? "│   " : "    ") << "├──"
		          << " " << m_op.value() << "\n";
		m_rhs->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	Token m_op;
	std::shared_ptr<Expr> m_rhs;
};

class BinaryExpr : public Expr
{
public:
	enum class BinaryOp
	{
		Plus,
		Minus,
	};

	BinaryExpr(std::shared_ptr<Expr> lhs, Token op, std::shared_ptr<Expr> rhs) :
	    m_lhs(lhs),
	    m_op(op),
	    m_rhs(rhs)
	{ }

	const char *name() const { return "BinaryExpr"; }
	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << name() << "\n";

		m_lhs->print(prefix + (is_left ? "│   " : "    "), true);
		std::cout << prefix << (is_left ? "│   " : "    ") << "├──"
		          << " " << m_op.value() << "\n";
		m_rhs->print(prefix + (is_left ? "│   " : "    "), false);
	}

private:
	std::shared_ptr<Expr> m_lhs;
	Token m_op;
	std::shared_ptr<Expr> m_rhs;
};

class CallExpr : public Expr
{
public:
	const char *name() const { return "CallExpr"; }
};

class Literal : public Expr
{
public:
	Literal(Value value) :
	    m_value(value)
	{ }

	const char *name() const { return "Literal"; }
	void print(std::string const &prefix, bool is_left)
	{
		std::cout << prefix;
		std::cout << (is_left ? "├──" : "└──");
		std::cout << " " << m_value.as_number() << "\n";
	}

private:
	Value m_value;
};
}
