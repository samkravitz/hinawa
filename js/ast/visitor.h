#pragma once

#include "../value.h"

namespace js
{
class BlockStmt;
class VariableStmt;
class EmptyStmt;
class IfStmt;
class ReturnStmt;
class ExpressionStmt;
class FunctionDecl;
class ForStmt;

class UnaryExpr;
struct UpdateExpr;
class BinaryExpr;
class CallExpr;
class Literal;
class Variable;

class StmtVisitor
{
public:
	virtual void visit(const BlockStmt *) const = 0;
	virtual void visit(const VariableStmt *) const = 0;
	virtual void visit(const EmptyStmt *) const = 0;
	virtual void visit(const IfStmt *) const = 0;
	virtual void visit(const ReturnStmt *) const = 0;
	virtual void visit(const ExpressionStmt *) const = 0;
	virtual void visit(const FunctionDecl *) const = 0;
	virtual void visit(const ForStmt *) const = 0;
};

class ExprVisitor
{
public:
	virtual Value visit(const UnaryExpr *) const = 0;
	virtual Value visit(const UpdateExpr *) const = 0;
	virtual Value visit(const BinaryExpr *) const = 0;
	virtual Value visit(const CallExpr *) const = 0;
	virtual Value visit(const Literal *) const = 0;
	virtual Value visit(const Variable *) const = 0;
};

class PrintVisitor
{
public:
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
	virtual void visit(const Literal *, int indent) const = 0;
	virtual void visit(const Variable *, int indent) const = 0;
};
}
