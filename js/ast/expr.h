#pragma once

#include <memory>
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
	virtual bool is_literal() const { return false; }
};

struct UnaryExpr : public Expr
{
	UnaryExpr(Token op, std::shared_ptr<Expr> rhs) :
	    op(op),
	    rhs(std::move(rhs))
	{ }

	const char *name() const { return "UnaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Token op;
	std::shared_ptr<Expr> rhs;
};

struct UpdateExpr : public Expr
{
	UpdateExpr(Token op, std::shared_ptr<Expr> operand, bool prefix) :
	    op(op),
	    operand(std::move(operand)),
	    prefix(prefix)
	{ }

	const char *name() const { return "UpdateExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Token op;
	std::shared_ptr<Expr> operand;
	bool prefix{false};
};

struct BinaryExpr : public Expr
{
	BinaryExpr(std::shared_ptr<Expr> lhs, Token op, std::shared_ptr<Expr> rhs) :
	    lhs(std::move(lhs)),
	    op(op),
	    rhs(std::move(rhs))
	{ }

	const char *name() const { return "BinaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> lhs;
	Token op;
	std::shared_ptr<Expr> rhs;
};

struct LogicalExpr : public Expr
{
	LogicalExpr(std::shared_ptr<Expr> lhs, Token op, std::shared_ptr<Expr> rhs) :
	    lhs(std::move(lhs)),
	    op(op),
	    rhs(std::move(rhs))
	{ }

	const char *name() const { return "LogicalExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> lhs;
	Token op;
	std::shared_ptr<Expr> rhs;
};

struct AssignmentExpr : public Expr
{
	AssignmentExpr(std::shared_ptr<Expr> lhs, Token op, std::shared_ptr<Expr> rhs) :
	    lhs(std::move(lhs)),
	    op(op),
	    rhs(std::move(rhs))
	{ }

	const char *name() const { return "AssignmentExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> lhs;
	Token op;
	std::shared_ptr<Expr> rhs;
};

struct CallExpr : public Expr
{
	CallExpr(std::shared_ptr<Expr> callee, std::vector<std::shared_ptr<Expr>> args) :
	    callee(callee),
	    args(args)
	{ }

	const char *name() const { return "CallExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> callee;
	std::vector<std::shared_ptr<Expr>> args;
};

struct MemberExpr : public Expr
{
	MemberExpr(std::shared_ptr<Expr> object, std::shared_ptr<Expr> property, bool is_dot = false) :
	    object(std::move(object)),
	    property(std::move(property)),
	    is_dot(is_dot)
	{ }

	const char *name() const { return "MemberExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
	bool is_member_expr() const { return true; }

	std::shared_ptr<Expr> object;
	std::shared_ptr<Expr> property;
	bool is_dot{false};
};

struct Literal : public Expr
{
	Literal(Token token) :
	    token(token)
	{ }

	const char *name() const { return "Literal"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
	bool is_literal() const { return true; }

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
	ObjectExpr(std::vector<std::pair<std::string, std::shared_ptr<Expr>>> properties) :
	    properties(properties)
	{ }

	const char *name() const { return "ObjectExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::pair<std::string, std::shared_ptr<Expr>>> properties;
};

struct FunctionExpr : public Expr
{
	FunctionExpr(std::string function_name, std::vector<std::string> args, std::shared_ptr<BlockStmt> body) :
	    function_name(function_name),
	    args(args),
	    body(std::move(body))
	{ }

	const char *name() const { return "FunctionExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	inline bool is_anonymous() const { return function_name == ""; }

	std::string function_name = "";
	std::vector<std::string> args;
	std::shared_ptr<BlockStmt> body;
};

struct NewExpr : public Expr
{
	NewExpr(std::shared_ptr<Expr> callee, std::vector<std::shared_ptr<Expr>> args) :
	    callee(std::move(callee)),
	    args(std::move(args))
	{ }

	const char *name() const { return "NewExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> callee;
	std::vector<std::shared_ptr<Expr>> args;
};

struct ArrayExpr : public Expr
{
	ArrayExpr(std::vector<std::shared_ptr<Expr>> elements) :
	    elements(elements)
	{ }

	const char *name() const { return "ArrayExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Expr>> elements;
};

struct TernaryExpr : public Expr
{
	TernaryExpr(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> if_true, std::shared_ptr<Expr> if_false) :
	    condition(std::move(condition)),
	    if_true(std::move(if_true)),
	    if_false(std::move(if_false))
	{ }

	const char *name() const { return "TernaryExpr"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> condition;
	std::shared_ptr<Expr> if_true;
	std::shared_ptr<Expr> if_false;
};
}
