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
	UnaryExpr(Token op, Expr *rhs) :
	    m_op(op),
	    m_rhs(rhs)
	{ }

	const char *name() const { return "UnaryExpr"; }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Expr *rhs() const { return m_rhs; }
	Token op() const { return m_op; };

private:
	Token m_op;
	Expr *m_rhs;
};

class BinaryExpr : public Expr
{
public:
	enum class BinaryOp
	{
		Plus,
		Minus,
	};

	BinaryExpr(Expr *lhs, Token op, Expr *rhs) :
	    m_lhs(lhs),
	    m_op(op),
	    m_rhs(rhs)
	{ }

	const char *name() const { return "BinaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }

	Expr *lhs() const { return m_lhs; }
	Token op() const { return m_op; };
	Expr *rhs() const { return m_rhs; }

	void generate_bytecode(Chunk &chunk) const
	{
		Opcode opcode;
		switch (op().type())
		{
			case PLUS: opcode = OP_ADD; break;
			case MINUS: opcode = OP_SUBTRACT; break;
			default: opcode = OP_UNKNOWN; break;
		}

		auto lhs_reg = chunk.allocate_register();
		auto rhs_reg = chunk.allocate_register();

		chunk.write(opcode, op().line());
		chunk.write(lhs_reg, op().line());
		chunk.write(rhs_reg, op().line());
	}

private:
	Expr *m_lhs;
	Token m_op;
	Expr *m_rhs;
};

class CallExpr : public Expr
{
public:
	CallExpr(Expr *callee, std::vector<Expr *> args) :
	    m_callee(callee),
	    m_args(args)
	{ }

	const char *name() const { return "CallExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }

	Expr *callee() const { return m_callee; }
	std::vector<Expr *> args() const { return m_args; }

private:
	Expr *m_callee;
	std::vector<Expr *> m_args;
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

class Variable : public Expr
{
public:
	Variable(std::string ident) :
	    m_ident(ident)
	{ }

	const char *name() const { return "Variable"; }
	Value accept(const ExprVisitor *visitor) const { return visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	std::string ident() const { return m_ident; }

private:
	std::string m_ident;
};
}
