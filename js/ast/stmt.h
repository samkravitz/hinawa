#pragma once

#include "ast.h"
#include "expr.h"
#include "visitor.h"

namespace js
{
struct Stmt : public AstNode
{
	const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual std::optional<size_t> accept(CompilerVisitor *compiler) const = 0;
};

//struct Program : public Stmt
//{
//public:
//	void add_stmt(Stmt * stmt) { stmts.push_back(stmt); }

//	const char *name() const { return "Program"; }
//	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
//i

//	std::vector<Stmt *> stmts() const { return stmts; }
//	void print(int indent, bool is_left)
//	{
//		std::cout << prefix;
//		std::cout << (is_left ? "├──" : "└──");
//		std::cout << name() << "\n";

//		for (auto stmt : stmts)
//			stmt->print(prefix + (is_left ? "│   " : "    "), true);
//	}

//private:
//	std::vector<Stmt *> stmts;
//};

struct BlockStmt : public Stmt
{
	BlockStmt(std::vector<Stmt *> stmts) :
	    stmts(stmts)
	{ }

	const char *name() const { return "BlockStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

	std::vector<Stmt *> stmts;
};

struct EmptyStmt : public Stmt
{
	const char *name() const { return "EmptyStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };
};

struct ExpressionStmt : public Stmt
{
	ExpressionStmt(Expr *expr) :
	    expr(expr)
	{ }

	const char *name() const { return "ExpressionStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

	Expr *expr;
};

struct IfStmt : public Stmt
{
	IfStmt(Expr *condition, Stmt *then, Stmt *else_stmt) :
	    condition(condition),
	    then(then),
	    else_stmt(else_stmt)
	{ }

	const char *name() const { return "IfStmt"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

	Expr *condition;
	Stmt *then;
	Stmt *else_stmt;
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
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

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
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

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
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

	std::string identifier;
	Expr *init;
};

struct FunctionDecl : public Stmt
{
	FunctionDecl(std::string function_name, Stmt *block) :
	    function_name(function_name),
	    block(block)
	{ }

	const char *name() const { return "FunctionDecl"; }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
	std::optional<size_t> accept(CompilerVisitor *compiler) const { return compiler->compile(*this); };

	std::string function_name;
	Stmt *block;
};

}
//struct TryStmt : public Stmt
//{
//public:
//	const char *name() const { return "TryStmt"; }
//};

//struct FunctionDeclaration : public AstNode
//{
//public:
//	const char *name() const { return "FunctionDeclaration"; }
//};

//struct VariableDeclaration : public AstNode
//{
//public:
//	const char *name() const { return "VariableDeclaration"; }
//};
