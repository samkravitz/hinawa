#pragma once

namespace js
{
struct BlockStmt;
struct VarDecl;
struct EmptyStmt;
struct IfStmt;
struct ReturnStmt;
struct ExpressionStmt;
struct FunctionDecl;
struct ForStmt;
struct ContinueStmt;
struct ThrowStmt;
struct TryStmt;

struct UnaryExpr;
struct UpdateExpr;
struct BinaryExpr;
struct LogicalExpr;
struct AssignmentExpr;
struct CallExpr;
struct MemberExpr;
struct Literal;
struct Variable;
struct ObjectExpr;
struct FunctionExpr;
struct NewExpr;
struct ArrayExpr;

struct PrintVisitor
{
	virtual void visit(const BlockStmt *, int indent) const = 0;
	virtual void visit(const VarDecl *, int indent) const = 0;
	virtual void visit(const EmptyStmt *, int indent) const = 0;
	virtual void visit(const IfStmt *, int indent) const = 0;
	virtual void visit(const ReturnStmt *, int indent) const = 0;
	virtual void visit(const ExpressionStmt *, int indent) const = 0;
	virtual void visit(const FunctionDecl *, int indent) const = 0;
	virtual void visit(const ForStmt *, int indent) const = 0;
	virtual void visit(const ContinueStmt *, int indent) const = 0;
	virtual void visit(const ThrowStmt *, int indent) const = 0;
	virtual void visit(const TryStmt *, int indent) const = 0;
	virtual void visit(const UnaryExpr *, int indent) const = 0;
	virtual void visit(const UpdateExpr *, int indent) const = 0;
	virtual void visit(const BinaryExpr *, int indent) const = 0;
	virtual void visit(const LogicalExpr *, int indent) const = 0;
	virtual void visit(const AssignmentExpr *, int indent) const = 0;
	virtual void visit(const CallExpr *, int indent) const = 0;
	virtual void visit(const MemberExpr *, int indent) const = 0;
	virtual void visit(const Literal *, int indent) const = 0;
	virtual void visit(const Variable *, int indent) const = 0;
	virtual void visit(const ObjectExpr *, int indent) const = 0;
	virtual void visit(const FunctionExpr *, int indent) const = 0;
	virtual void visit(const NewExpr *, int indent) const = 0;
	virtual void visit(const ArrayExpr *, int indent) const = 0;
};

struct CompilerVisitor
{
	virtual void compile(const BlockStmt &) = 0;
	virtual void compile(const VarDecl &) = 0;
	virtual void compile(const EmptyStmt &) = 0;
	virtual void compile(const IfStmt &) = 0;
	virtual void compile(const ReturnStmt &) = 0;
	virtual void compile(const ExpressionStmt &) = 0;
	virtual void compile(const FunctionDecl &) = 0;
	virtual void compile(const ForStmt &) = 0;
	virtual void compile(const ContinueStmt &) = 0;
	virtual void compile(const ThrowStmt &) = 0;
	virtual void compile(const TryStmt &) = 0;
	virtual void compile(const UnaryExpr &) = 0;
	virtual void compile(const UpdateExpr &) = 0;
	virtual void compile(const BinaryExpr &) = 0;
	virtual void compile(const LogicalExpr &) = 0;
	virtual void compile(const AssignmentExpr &) = 0;
	virtual void compile(const CallExpr &) = 0;
	virtual void compile(const MemberExpr &) = 0;
	virtual void compile(const Literal &) = 0;
	virtual void compile(const Variable &) = 0;
	virtual void compile(const ObjectExpr &) = 0;
	virtual void compile(const FunctionExpr &) = 0;
	virtual void compile(const NewExpr &) = 0;
	virtual void compile(const ArrayExpr &) = 0;
};
}
