#pragma once

#include <vector>

#include "ast.h"
#include "expr.h"
#include "visitor.h"

namespace js
{
struct Stmt : public AstNode
{
	const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual void accept(CompilerVisitor *compiler) const = 0;
};

struct BlockStmt : public Stmt
{
	BlockStmt(std::vector<Stmt *> stmts) :
	    stmts(stmts)
	{ }

	const char *name() const { return "BlockStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<Stmt *> stmts;
};

struct EmptyStmt : public Stmt
{
	const char *name() const { return "EmptyStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
};

struct ExpressionStmt : public Stmt
{
	ExpressionStmt(Expr *expr) :
	    expr(expr)
	{ }

	const char *name() const { return "ExpressionStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *expr;
};

struct IfStmt : public Stmt
{
	IfStmt(Expr *test, Stmt *consequence, Stmt *alternate) :
	    test(test),
	    consequence(consequence),
	    alternate(alternate)
	{ }

	const char *name() const { return "IfStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *test;
	Stmt *consequence;
	Stmt *alternate{nullptr};
};

struct ForStmt : public Stmt
{
	ForStmt(AstNode *initialization, Expr *condition, Expr *afterthought, Stmt *statement) :
	    initialization(initialization),
	    condition(condition),
	    afterthought(afterthought),
	    statement(statement)
	{ }

	const char *name() const { return "ForStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	AstNode *initialization;
	Expr *condition;
	Expr *afterthought;
	Stmt *statement;
};

//struct ContinueStmt : public Stmt
//{
//public:
//	const char *name() const { return "ContinueStmt"; }
//};

//struct BreakStmt : public Stmt
//{
//public:
//	const char *name() const { return "BreakStmt"; }
//};

struct ReturnStmt : public Stmt
{
	ReturnStmt(Expr *expr) :
	    expr(expr)
	{ }

	const char *name() const { return "ReturnStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *expr;
};

struct VarDecl : public Stmt
{
	VarDecl(std::string identifier, Expr *initializer) :
	    identifier(identifier),
	    init(initializer)
	{ }
	const char *name() const { return "VarDecl"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::string identifier;
	Expr *init;
};

struct FunctionDecl : public Stmt
{
	FunctionDecl(std::string function_name, std::vector<std::string> args, BlockStmt *block) :
	    function_name(function_name),
	    args(args),
	    block(block)
	{ }

	const char *name() const { return "FunctionDecl"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::string function_name;
	std::vector<std::string> args;
	BlockStmt *block;
};

struct ThrowStmt : public Stmt
{
	ThrowStmt(Expr *expr) :
	    expr(expr)
	{ }

	const char *name() const { return "ThrowStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	Expr *expr;
};

struct TryStmt : public Stmt
{
	TryStmt(BlockStmt *block, BlockStmt *handler, BlockStmt *finalizer, std::optional<std::string> catch_param) :
	    block(block),
	    handler(handler),
	    finalizer(finalizer),
	    catch_param(catch_param)
	{ }

	const char *name() const { return "TryStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	BlockStmt *block;
	BlockStmt *handler{nullptr};
	BlockStmt *finalizer{nullptr};
	std::optional<std::string> catch_param = {};
};
}
