#pragma once

#include "../value.h"

namespace js
{
class Program;
class BlockStmt;
class VariableStmt;
class EmptyStmt;
class IfStmt;
class ReturnStmt;
class ExpressionStmt;

class UnaryExpr;
class BinaryExpr;
class CallExpr;
class Literal;

class StmtVisitor
{
public:
	virtual void visit(const Program *) const = 0;
	virtual void visit(const BlockStmt *) const = 0;
	virtual void visit(const VariableStmt *) const = 0;
	virtual void visit(const EmptyStmt *) const = 0;
	virtual void visit(const IfStmt *) const = 0;
	virtual void visit(const ReturnStmt *) const = 0;
	virtual void visit(const ExpressionStmt *) const = 0;
};

class ExprVisitor
{
public:
	virtual Value visit(const UnaryExpr *) const = 0;
	virtual Value visit(const BinaryExpr *) const = 0;
	virtual Value visit(const CallExpr *) const = 0;
	virtual Value visit(const Literal *) const = 0;
};
}
