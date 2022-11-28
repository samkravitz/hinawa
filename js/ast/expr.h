#pragma once

#include "../token.h"
#include "../value.h"
#include "ast.h"
#include "visitor.h"

namespace js
{
class Expr : public AstNode
{
public:
	virtual const char *name() const = 0;
	virtual Value accept(const ExprVisitor *visitor) const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
};

class UnaryExpr : public Expr
{
public:
	UnaryExpr(Token op, std::shared_ptr<Expr> rhs) :
	    m_op(op),
	    m_rhs(rhs)
	{ }

	const char *name() const { return "UnaryExpr"; }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	std::shared_ptr<Expr> rhs() const { return m_rhs; }
	Token op() const { return m_op; };

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
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }

	std::shared_ptr<Expr> lhs() const { return m_lhs; }
	Token op() const { return m_op; };
	std::shared_ptr<Expr> rhs() const { return m_rhs; }

private:
	std::shared_ptr<Expr> m_lhs;
	Token m_op;
	std::shared_ptr<Expr> m_rhs;
};

class CallExpr : public Expr
{
public:
	const char *name() const { return "CallExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
};

class Literal : public Expr
{
public:
	Literal(Value value) :
	    m_value(value)
	{ }

	const char *name() const { return "Literal"; }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	Value value() const { return m_value; }

private:
	Value m_value;
};
}
