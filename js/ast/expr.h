#pragma once

#include "../token.h"
#include "../value.h"
#include "ast.h"
#include "visitor.h"

namespace js
{
struct Expr : public AstNode
{
public:
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
};

struct UnaryExpr : public Expr
{
	UnaryExpr(Token op, Expr *rhs) :
	    op(op),
	    rhs(rhs)
	{ }

	const char *name() const { return "UnaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Token op;
	Expr *rhs;
};

struct UpdateExpr : public Expr
{
	UpdateExpr(Token op, Expr *operand, bool prefix) :
	    op(op),
	    operand(operand),
	    prefix(prefix)
	{ }

	const char *name() const { return "UpdateExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Token op;
	Expr *operand;
	bool prefix{ false };
};

struct BinaryExpr : public Expr
{
	BinaryExpr(Expr *lhs, Token op, Expr *rhs) :
	    lhs(lhs),
	    op(op),
	    rhs(rhs)
	{ }

	const char *name() const { return "BinaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Expr *lhs;
	Token op;
	Expr *rhs;
};

struct CallExpr : public Expr
{
	CallExpr(Expr *callee, std::vector<Expr *> args) :
	    callee(callee),
	    args(args)
	{ }

	const char *name() const { return "CallExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Expr *callee;
	std::vector<Expr *> args;
};

struct MemberExpr : public Expr
{
	MemberExpr(Expr *object, Token property_name) :
	    object(object),
	    property_name(property_name)
	{ }

	const char *name() const { return "MemberExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Expr *object;
	Token property_name;
};

struct Literal : public Expr
{
	Literal(Value value) :
	    value(value)
	{ }

	const char *name() const { return "Literal"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Value value;
};

struct Variable : public Expr
{
	Variable(std::string ident) :
	    ident(ident)
	{ }

	const char *name() const { return "Variable"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	std::string ident;
};
}
