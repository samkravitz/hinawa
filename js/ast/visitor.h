#pragma once

namespace js
{
struct BlockStmt;
struct VariableStmt;
struct EmptyStmt;
struct IfStmt;
struct ReturnStmt;
struct ExpressionStmt;
struct FunctionDecl;
struct ForStmt;

struct UnaryExpr;
struct UpdateExpr;
struct BinaryExpr;
struct CallExpr;
struct MemberExpr;
struct Literal;
struct Variable;

struct PrintVisitor
{
	virtual void visit(const BlockStmt *, int indent) const = 0;
	virtual void visit(const VariableStmt *, int indent) const = 0;
	virtual void visit(const EmptyStmt *, int indent) const = 0;
	virtual void visit(const IfStmt *, int indent) const = 0;
	virtual void visit(const ReturnStmt *, int indent) const = 0;
	virtual void visit(const ExpressionStmt *, int indent) const = 0;
	virtual void visit(const FunctionDecl *, int indent) const = 0;
	virtual void visit(const ForStmt *, int indent) const = 0;
	virtual void visit(const UnaryExpr *, int indent) const = 0;
	virtual void visit(const UpdateExpr *, int indent) const = 0;
	virtual void visit(const BinaryExpr *, int indent) const = 0;
	virtual void visit(const CallExpr *, int indent) const = 0;
	virtual void visit(const MemberExpr *, int indent) const = 0;
	virtual void visit(const Literal *, int indent) const = 0;
	virtual void visit(const Variable *, int indent) const = 0;
};

struct CompilerVisitor
{
	virtual void compile(const BlockStmt &) = 0;
	virtual void compile(const VariableStmt &) = 0;
	virtual void compile(const EmptyStmt &) = 0;
	virtual void compile(const IfStmt &) = 0;
	virtual void compile(const ReturnStmt &) = 0;
	virtual void compile(const ExpressionStmt &) = 0;
	virtual void compile(const FunctionDecl &) = 0;
	virtual void compile(const ForStmt &) = 0;
	virtual void compile(const UnaryExpr &) = 0;
	virtual void compile(const UpdateExpr &) = 0;
	virtual void compile(const BinaryExpr &) = 0;
	virtual void compile(const CallExpr &) = 0;
	virtual void compile(const MemberExpr &) = 0;
	virtual void compile(const Literal &) = 0;
	virtual void compile(const Variable &) = 0;
};
}
