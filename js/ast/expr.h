#pragma once

#include <vector>

#include "../token.h"
#include "ast.h"
#include "visitor.h"

namespace js
{
struct Expr : public AstNode
{
public:
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual void accept(CompilerVisitor *compiler) const = 0;
	virtual bool is_variable() const { return false; }
	virtual bool is_member_expr() const { return false; }
};

struct UnaryExpr : public Expr
{
	UnaryExpr(Token op, Expr *rhs) :
	    op(op),
	    rhs(rhs)
	{ }

	const char *name() const { return "UnaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

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
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Token op;
	Expr *operand;
	bool prefix{false};
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
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *lhs;
	Token op;
	Expr *rhs;
};

struct AssignmentExpr : public Expr
{
	AssignmentExpr(Expr *lhs, Expr *rhs) :
	    lhs(lhs),
	    rhs(rhs)
	{ }

	const char *name() const { return "AssignmentExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *lhs;
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
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *callee;
	std::vector<Expr *> args;
};

struct MemberExpr : public Expr
{
	MemberExpr(Expr *object, std::string property_name) :
	    object(object),
	    property_name(property_name)
	{ }

	const char *name() const { return "MemberExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
	bool is_member_expr() const { return true; }

	Expr *object;
	std::string property_name;
};

struct Literal : public Expr
{
	Literal(Token token) :
	    token(token)
	{ }

	const char *name() const { return "Literal"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Token token;
};

struct Variable : public Expr
{
	Variable(std::string ident) :
	    ident(ident)
	{ }

	const char *name() const { return "Variable"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
	bool is_variable() const { return true; }

	std::string ident;
};

struct ObjectExpr : public Expr
{
	ObjectExpr(std::vector<std::pair<std::string, Expr *>> properties) :
	    properties(properties)
	{ }

	const char *name() const { return "ObjectExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::pair<std::string, Expr *>> properties;
};

struct FunctionExpr : public Expr
{
	FunctionExpr(std::vector<std::string> args, BlockStmt *body) :
	    args(args),
	    body(body)
	{ }

	const char *name() const { return "FunctionExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::string> args;
	BlockStmt *body;
};

struct NewExpr : public Expr
{
	NewExpr(Expr *callee, std::vector<Expr *> params) :
	    callee(callee),
	    params(params)
	{ }

	const char *name() const { return "NewExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *callee;
	std::vector<Expr *> params;
};
}
