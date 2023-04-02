#pragma once

#include "ast.h"
#include "expr.h"
#include "visitor.h"

namespace js
{
class Stmt : public AstNode
{
public:
	const char *name() const = 0;
	virtual void accept(const StmtVisitor *visitor) const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
};

//class Program : public Stmt
//{
//public:
//	void add_stmt(Stmt * stmt) { m_stmts.push_back(stmt); }

//	const char *name() const { return "Program"; }
//	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
//i

//	std::vector<Stmt *> stmts() const { return m_stmts; }
//	void print(int indent, bool is_left)
//	{
//		std::cout << prefix;
//		std::cout << (is_left ? "├──" : "└──");
//		std::cout << name() << "\n";

//		for (auto stmt : m_stmts)
//			stmt->print(prefix + (is_left ? "│   " : "    "), true);
//	}

//private:
//	std::vector<Stmt *> m_stmts;
//};

class BlockStmt : public Stmt
{
public:
	BlockStmt(std::vector<Stmt *> stmts) :
	    m_stmts(stmts)
	{ }

	const char *name() const { return "BlockStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };

	inline std::vector<Stmt *> stmts() const { return m_stmts; }

private:
	std::vector<Stmt *> m_stmts;
};

class VariableStmt : public Stmt
{
public:
	VariableStmt(std::string identifier, Expr *initializer) :
	    m_identifier(identifier),
	    m_init(initializer)
	{ }
	const char *name() const { return "VariableStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };

	std::string identifier() const { return m_identifier; }
	Expr *init() const { return m_init; }

private:
	std::string m_identifier;
	Expr *m_init;
};

class EmptyStmt : public Stmt
{
public:
	const char *name() const { return "EmptyStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };
};

class ExpressionStmt : public Stmt
{
public:
	ExpressionStmt(Expr *expr) :
	    m_expr(expr)
	{ }

	const char *name() const { return "ExpressionStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); };

	Expr *expr() const { return m_expr; }

private:
	Expr *m_expr;
};

class IfStmt : public Stmt
{
public:
	IfStmt(Expr *condition, Stmt *then, Stmt *else_stmt) :
	    m_condition(condition),
	    m_then(then),
	    m_else(else_stmt)
	{ }

	Expr *condition() const { return m_condition; }
	Stmt *then() const { return m_then; }
	Stmt *else_stmt() const { return m_else; }
	const char *name() const { return "IfStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
private:
	Expr *m_condition;
	Stmt *m_then;
	Stmt *m_else;
};

class ForStmt : public Stmt
{
public:
	ForStmt(AstNode *initialization, Expr *condition, Expr *afterthought, Stmt *statement) :
	    m_initialization(initialization),
	    m_condition(condition),
	    m_afterthought(afterthought),
		m_statement(statement)
	{ }

	AstNode *initialization() const { return m_initialization; }
	Expr *condition() const { return m_condition; }
	Expr *afterthought() const { return m_afterthought; }
	Stmt *statement() const { return m_statement; }
	const char *name() const { return "ForStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }
private:
	AstNode *m_initialization;
	Expr *m_condition;
	Expr *m_afterthought;
	Stmt *m_statement;
};

//class ContinueStmt : public Stmt
//{
//public:
//	const char *name() const { return "ContinueStmt"; }
//};

//class BreakStmt : public Stmt
//{
//public:
//	const char *name() const { return "BreakStmt"; }
//};

class ReturnStmt : public Stmt
{
public:
	ReturnStmt(Expr *expr) :
	    m_expr(expr)
	{ }

	const char *name() const { return "ReturnStmt"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	Expr *expr() const { return m_expr; }

private:
	Expr *m_expr;
};

class FunctionDecl : public Stmt
{
public:
	FunctionDecl(std::string function_name, Stmt *block) :
	    m_function_name(function_name),
	    m_block(block)
	{ }

	const char *name() const { return "FunctionDecl"; }
	void accept(const StmtVisitor *visitor) const { visitor->visit(this); }
	void accept(const PrintVisitor *visitor, int indent) const { visitor->visit(this, indent); }

	std::string function_name() const { return m_function_name; }
	Stmt *block() const { return m_block; }

private:
	std::string m_function_name;
	Stmt *m_block;
};

}
//class TryStmt : public Stmt
//{
//public:
//	const char *name() const { return "TryStmt"; }
//};

//class FunctionDeclaration : public AstNode
//{
//public:
//	const char *name() const { return "FunctionDeclaration"; }
//};

//class VariableDeclaration : public AstNode
//{
//public:
//	const char *name() const { return "VariableDeclaration"; }
//};