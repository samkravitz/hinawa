#pragma once

#include <algorithm>
#include <optional>
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
	virtual bool is_var_decl() { return false; }

	virtual std::vector<std::shared_ptr<Stmt>> remove_var_decls() { return {}; }
};

struct BlockStmt : public Stmt
{
	BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) :
	    stmts(stmts)
	{ }

	const char *name() const { return "BlockStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override
	{
		std::vector<std::shared_ptr<Stmt>> var_decls;
		for (auto stmt : stmts)
		{
			auto other = stmt->remove_var_decls();
			var_decls.insert(var_decls.begin(), other.begin(), other.end());
		}

		stmts.erase(std::remove_if(stmts.begin(), stmts.end(), [](auto stmt) { return stmt->is_var_decl(); }),
		            stmts.end());

		return var_decls;
	}

	std::vector<std::shared_ptr<Stmt>> stmts;
};

struct ScopeNode : public Stmt
{
	ScopeNode(std::shared_ptr<Stmt> stmt) :
	    stmt(stmt)
	{ }

	const char *name() const { return "ScopeNode"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Stmt> stmt;
};

struct EmptyStmt : public Stmt
{
	const char *name() const { return "EmptyStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };
};

struct ExpressionStmt : public Stmt
{
	ExpressionStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char *name() const { return "ExpressionStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> expr;
};

struct IfStmt : public Stmt
{
	IfStmt(std::shared_ptr<Expr> test, std::shared_ptr<Stmt> consequence, std::shared_ptr<Stmt> alternate) :
	    test(std::move(test)),
	    consequence(std::move(consequence)),
	    alternate(std::move(alternate))
	{ }

	const char *name() const { return "IfStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override
	{
		std::vector<std::shared_ptr<Stmt>> var_decls;

		auto other = consequence->remove_var_decls();
		var_decls.insert(var_decls.begin(), other.begin(), other.end());

		if (consequence)
		{
			other = consequence->remove_var_decls();
			var_decls.insert(var_decls.begin(), other.begin(), other.end());
		}

		return var_decls;
	}

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

	const char *name() const { return "ForStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override
	{
		std::vector<std::shared_ptr<Stmt>> var_decls;

		if (initialization)
		{
			auto stmt = dynamic_pointer_cast<Stmt>(initialization);
			if (stmt && stmt->is_var_decl())
			{
				var_decls.push_back(stmt);
				initialization = nullptr;
			}
		}

		return var_decls;
	}

	std::shared_ptr<AstNode> initialization;
	std::shared_ptr<Expr> condition;
	std::shared_ptr<Expr> afterthought;
	std::shared_ptr<Stmt> statement;
};

struct WhileStmt : public Stmt
{
	WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> statement) :
	    condition(std::move(condition)),
	    statement(std::move(statement))
	{ }

	const char *name() const { return "WhileStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override
	{
		std::vector<std::shared_ptr<Stmt>> var_decls;

		auto other = statement->remove_var_decls();
		var_decls.insert(var_decls.begin(), other.begin(), other.end());

		return var_decls;
	}

	std::shared_ptr<Expr> condition;
	std::shared_ptr<Stmt> statement;
};

struct ContinueStmt : public Stmt
{
	ContinueStmt(std::string label = "") :
	    label(label)
	{ }

	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	const char *name() const { return "ContinueStmt"; }

	std::string label = "";
};

struct BreakStmt : public Stmt
{
	BreakStmt(std::string label = "") :
	    label(label)
	{ }

	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	const char *name() const { return "BreakStmt"; }

	std::string label = "";
};

struct DebuggerStmt : public Stmt
{
	DebuggerStmt() = default;

	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	const char *name() const { return "DebuggerStmt"; }
};

struct ReturnStmt : public Stmt
{
	ReturnStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char *name() const { return "ReturnStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::shared_ptr<Expr> expr;
};

struct VarDecl : public Stmt
{
	enum VarDeclKind
	{
		VAR,
		LET,
		CONST,
	};

	struct VarDeclarator
	{
		VarDeclarator(std::string identifier, std::shared_ptr<Expr> initializer) :
		    identifier(identifier),
		    init(std::move(initializer))
		{ }

		std::string identifier;
		std::shared_ptr<Expr> init;
	};

	VarDecl(std::vector<VarDeclarator> declorators, VarDeclKind kind) :
	    declorators(declorators),
	    kind(kind)
	{ }

	const char *name() const { return "VarDecl"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	bool is_constant() const { return kind == CONST; }

	bool is_var_decl() override { return kind == VAR; }

	std::vector<VarDeclarator> declorators;
	VarDeclKind kind;
};

struct FunctionDecl : public Stmt
{
	FunctionDecl(std::string function_name, std::vector<std::string> args, std::shared_ptr<BlockStmt> block) :
	    function_name(function_name),
	    args(args),
	    block(std::move(block))
	{ }

	const char *name() const { return "FunctionDecl"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override { return block->remove_var_decls(); }

	std::string function_name;
	std::vector<std::string> args;
	std::shared_ptr<BlockStmt> block;
};

struct ThrowStmt : public Stmt
{
	ThrowStmt(std::shared_ptr<Expr> expr) :
	    expr(std::move(expr))
	{ }

	const char *name() const { return "ThrowStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

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

	const char *name() const { return "TryStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	void accept(CompilerVisitor *compiler) const { compiler->compile(*this); };

	std::vector<std::shared_ptr<Stmt>> remove_var_decls() override
	{
		std::vector<std::shared_ptr<Stmt>> var_decls;

		auto other = block->remove_var_decls();
		var_decls.insert(var_decls.begin(), other.begin(), other.end());

		if (handler)
		{
			other = handler->remove_var_decls();
			var_decls.insert(var_decls.begin(), other.begin(), other.end());
		}

		if (finalizer)
		{
			other = finalizer->remove_var_decls();
			var_decls.insert(var_decls.begin(), other.begin(), other.end());
		}

		return var_decls;
	}

	std::shared_ptr<BlockStmt> block;
	std::shared_ptr<BlockStmt> handler{nullptr};
	std::shared_ptr<BlockStmt> finalizer{nullptr};
	std::optional<std::string> catch_param = {};
};
}
