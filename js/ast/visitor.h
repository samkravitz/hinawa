#pragma once

#include <optional>

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

struct UnaryExpr;
struct UpdateExpr;
struct BinaryExpr;
struct AssignmentExpr;
struct CallExpr;
struct MemberExpr;
struct Literal;
struct Variable;

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
	virtual void visit(const UnaryExpr *, int indent) const = 0;
	virtual void visit(const UpdateExpr *, int indent) const = 0;
	virtual void visit(const BinaryExpr *, int indent) const = 0;
	virtual void visit(const AssignmentExpr *, int indent) const = 0;
	virtual void visit(const CallExpr *, int indent) const = 0;
	virtual void visit(const MemberExpr *, int indent) const = 0;
	virtual void visit(const Literal *, int indent) const = 0;
	virtual void visit(const Variable *, int indent) const = 0;
};

struct CompilerVisitor
{
	virtual std::optional<size_t> compile(const BlockStmt &) = 0;
	virtual std::optional<size_t> compile(const VarDecl &) = 0;
	virtual std::optional<size_t> compile(const EmptyStmt &) = 0;
	virtual std::optional<size_t> compile(const IfStmt &) = 0;
	virtual std::optional<size_t> compile(const ReturnStmt &) = 0;
	virtual std::optional<size_t> compile(const ExpressionStmt &) = 0;
	virtual std::optional<size_t> compile(const FunctionDecl &) = 0;
	virtual std::optional<size_t> compile(const ForStmt &) = 0;
	virtual std::optional<size_t> compile(const UnaryExpr &) = 0;
	virtual std::optional<size_t> compile(const UpdateExpr &) = 0;
	virtual std::optional<size_t> compile(const BinaryExpr &) = 0;
	virtual std::optional<size_t> compile(const AssignmentExpr &) = 0;
	virtual std::optional<size_t> compile(const CallExpr &) = 0;
	virtual std::optional<size_t> compile(const MemberExpr &) = 0;
	virtual std::optional<size_t> compile(const Literal &) = 0;
	virtual std::optional<size_t> compile(const Variable &) = 0;
};
}
