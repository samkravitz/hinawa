#pragma once

#include <optional>
#include <vector>

#include "ast.h"
#include "expr.h"
#include "visitor.h"

namespace js
{
struct Stmt : public AstNode
{
	const char* name() const = 0;
	virtual void accept(const PrintVisitor* visitor, int indent) const = 0;
	virtual void accept(CompilerVisitor* compiler) const = 0;
};

struct BlockStmt : public Stmt
{
	BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) :
	    stmts(stmts)
	{ }

	const char* name() const { return "BlockStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> stmts;
};

struct EmptyStmt : public Stmt
{
	const char* name() const { return "EmptyStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };
};

struct ExpressionStmt : public Stmt
{
	ExpressionStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char* name() const { return "ExpressionStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> expr;
};

struct IfStmt : public Stmt
{
	IfStmt(std::shared_ptr<Expr> test, std::shared_ptr<Stmt> consequence, std::shared_ptr<Stmt> alternate) :
	    test(std::move(test)),
	    consequence(std::move(consequence)),
	    alternate(std::move(alternate))
	{ }

	const char* name() const { return "IfStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> test;
	std::shared_ptr<Stmt> consequence;
	std::shared_ptr<Stmt> alternate{nullptr};
};

struct ForStmt : public Stmt
{
	ForStmt(std::shared_ptr<AstNode> initialization,
	        std::shared_ptr<Expr> condition,
	        std::shared_ptr<Expr> afterthought,
	        std::shared_ptr<Stmt> statement) :
	    initialization(initialization),
	    condition(std::move(condition)),
	    afterthought(std::move(afterthought)),
	    statement(std::move(statement))
	{ }

	const char* name() const { return "ForStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<AstNode> initialization;
	std::shared_ptr<Expr> condition;
	std::shared_ptr<Expr> afterthought;
	std::shared_ptr<Stmt> statement;
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
	ReturnStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char* name() const { return "ReturnStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> expr;
};

struct VarDecl : public Stmt
{
	struct VarDeclarator
	{
		VarDeclarator(std::string identifier, std::shared_ptr<Expr> initializer) :
		    identifier(identifier),
		    init(std::move(initializer))
		{ }

		std::string identifier;
		std::shared_ptr<Expr> init;
	};

	VarDecl(std::vector<VarDeclarator> declorators) :
	    declorators(declorators)
	{ }

	const char* name() const { return "VarDecl"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::vector<VarDeclarator> declorators;
};

struct FunctionDecl : public Stmt
{
	FunctionDecl(std::string function_name, std::vector<std::string> args, std::shared_ptr<BlockStmt> block) :
	    function_name(function_name),
	    args(args),
	    block(std::move(block))
	{ }

	const char* name() const { return "FunctionDecl"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::string function_name;
	std::vector<std::string> args;
	std::shared_ptr<BlockStmt> block;
};

struct ThrowStmt : public Stmt
{
	ThrowStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char* name() const { return "ThrowStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> expr;
};

struct TryStmt : public Stmt
{
	TryStmt(std::shared_ptr<BlockStmt> block,
	        std::shared_ptr<BlockStmt> handler,
	        std::shared_ptr<BlockStmt> finalizer,
	        std::optional<std::string> catch_param) :
	    block(std::move(block)),
	    handler(std::move(handler)),
	    finalizer(std::move(finalizer)),
	    catch_param(catch_param)
	{ }

	const char* name() const { return "TryStmt"; }
	void accept(const PrintVisitor* visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor* compiler) const { compiler->compile(*this); };

	std::shared_ptr<BlockStmt> block;
	std::shared_ptr<BlockStmt> handler{nullptr};
	std::shared_ptr<BlockStmt> finalizer{nullptr};
	std::optional<std::string> catch_param = {};
};
}
